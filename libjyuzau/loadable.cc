/* Copyright 2014 Mo McRoberts.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <OGRE/OgreLogManager.h>
#include <OGRE/OgreColourValue.h>

#include "jyuzau/loadable.hh"

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
# include <OGRE/OSX/macUtils.h>
#endif

using namespace Jyuzau;

/* Method flow:
 *
 * Loadable::Loadable()                 [public constructor]
 * Loadable::load()                     [public]
 *  Loadable::loadDocument()
 *   Loadable::startElement()           [via SAX callbacks]
 *    Loadable::factory()
 *     [LoadableObject or descendant constructor]
 *    Loadable::add()
 *   Loadable::endElement()             [via SAX callbacks]
 *    LoadableObject::didFinishLoding() [may recurse the tree]
 *  Loadable::complete()
 *   LoadableObject::complete()         [may recurse the tree]
 *  Loadable::didFinishLoading()
 *   Loadable::addResources()
 *    LoadableObject::addResources()    [may recurse the tree]
 *  Loadable::discard()
 *   delete LoadableObject              [if root is discardable, or...]
 *   LoadableObject::discard()          [may recurse the tree]
 * 
 * Descendants will typically include an attach() method which attaches the
 * asset to the scene (or in the case of a Scene, attaches the scene to an
 * Ogre::SceneManager).
 */

Loadable::Loadable():
	m_name(""),
	m_kind(""),
	m_container(""),
	m_path(""),
	m_loaded(false),
	m_load_status(false),
	m_root(NULL),
	m_cur(NULL),
	m_objects(),
	m_owner(NULL),
	m_unique(false),
	m_state(NULL)
{
}

/* Note that m_owner is not copied from the source instance */
Loadable::Loadable(const Loadable &object):
	m_name(""),
	m_kind(""),
	m_container(""),
	m_path(""),
	m_loaded(false),
	m_load_status(false),
	m_root(NULL),
	m_cur(NULL),
	m_objects(),
	m_owner(NULL),
	m_unique(false),
	m_state(NULL)
{
	if(!object.m_loaded || !object.m_load_status)
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: cannot duplicate a loadable instance which has an incomplete definition");
		return;
	}
	m_name = object.m_name;
	m_kind = object.m_kind;
	m_container = object.m_container;
	m_path = object.m_path;
	m_loaded = object.m_loaded;
	m_load_status = object.m_load_status;
	m_unique = object.m_unique;
	m_state = object.m_state;
}

Loadable::Loadable(Ogre::String name, State *state, Ogre::String kind, bool subdir):
	m_name(name),
	m_kind(kind),
	m_container(""),
	m_path(""),
	m_loaded(false),
	m_load_status(false),
	m_root(NULL),
	m_cur(NULL),
	m_objects(),
	m_owner(NULL),
	m_unique(false),
	m_state(state)
{
	Ogre::String base;
	
	if (!Ogre::StringUtil::startsWith(name, "/", false))
	{
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
		base = Ogre::String(Ogre::macBundlePath() + "/Contents/Resources/assets/" + kind + "s");
#else
		base = Ogre::String("assets/" + kind + "s");
#endif
	}
	if(subdir)
	{
		m_container = Ogre::String(base + "/" + name);
		m_path = Ogre::String(m_container + "/" + kind + ".xml");
	}
	else
	{
		m_container = base;
		m_path = m_container + "/" + name + ".xml";
	}
	m_group = Ogre::String(m_kind + "::" + m_name);
}

Loadable::~Loadable()
{
	std::vector<Loadable *>::iterator it;
	
	for(it = m_objects.begin(); it != m_objects.end(); it++)
	{
		delete *it;
	}
	if(m_root)
	{
		delete m_root;
	}
}

Loadable *
Loadable::clone(void)
{
	return new Loadable(*this);
}

/* Property handlers */
Ogre::String
Loadable::name(void) const
{
	return m_name;
}

Ogre::String
Loadable::kind(void) const
{
	return m_kind;
}

LoadableObject *
Loadable::root(void) const
{
	return m_root;
}

/* A unique object is never added to a state's object pool */
bool
Loadable::unique(void) const
{
	return m_unique;
}

State *
Loadable::state(void) const
{
	return m_state;
}

/* The load() method is the public method for loading an asset and checking
 * (via LoadableObject::complete()) than the asset definition wasn't
 * incomplete.
 */
bool
Loadable::load(void)
{
	if(m_loaded)
	{
		return m_load_status;
	}
	m_loaded = true;
	m_load_status = false;
	if(!m_path.length())
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: failed to load " + m_name + "[" + m_kind + "] which has no path");
		return false;
	}
	if(!loadDocument(m_path))
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: failed to load " + m_name + "[" + m_kind + "] from " + m_path);
		return false;
	}
	if(!m_root)
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: no suitable root found in " + m_path);
		return false;
	}
	if(!m_root->complete())
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: " + m_name + "[" + m_kind + "] from " + m_path + " has an incomplete definition");
		discard();
		return false;
	}
	m_load_status = true;
	didFinishLoading();
	discard();
	return true;
}

/* didFinishLoading() is invoked by load() once the document has been
 * parsed and the LoadableObject tree has been constructed. By default,
 * loaded() invokes addResources().
 */
void
Loadable::didFinishLoading(void)
{
	if(!addResources(m_group))
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: failed to add resources from " + m_group);
		m_load_status = false;
		return;
	}
}

/* Invoked by didFinishLoading() to trigger the LoadableObject tree to load or
 * create any associated resources. Note at this point there is no scene
 * manager to attach objects to.
 */
bool
Loadable::addResources(Ogre::String group)
{
	if(m_root && !m_root->addResources(group))
	{
		return false;
	}
	return true;
}

/* Destroy any LoadableObjects marked as discardable. Invoked by load() after
 * loaded() has completed post-load actions.
 */
void
Loadable::discard(void)
{
	if(m_root->m_discardable)
	{
		delete m_root;
		m_root = NULL;
		m_cur = NULL;
	}
	else
	{
		m_root->discard();
	}
}

/* Invoked by startElement() to create a new LoadableObject to add to the
 * tree.
 */
LoadableObject *
Loadable::factory(Ogre::String name, AttrList &attrs)
{
	/* Default loadable object factory; simply returns a new instance
	 * of LoadableObject
	 */
	return new LoadableObject(this, m_root, name, attrs);
}

/* Invoked by load() to parse the XML document for this asset */
bool
Loadable::loadDocument(Ogre::String path)
{
	xmlParserCtxtPtr ctx;
	xmlSAXHandler sax;
	xmlDocPtr doc;
	
	m_skip = 0;
	memset(&sax, 0, sizeof(sax));
	sax.initialized = XML_SAX2_MAGIC;
	sax.startElementNs = sax_startElement;
	sax.endElementNs = sax_endElement;
	ctx = xmlCreatePushParserCtxt(&(sax), (void *) this, "", 0, NULL);
	if(!ctx)
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: failed to create XML parser context");
		return false;
	}
	xmlCtxtUseOptions(ctx, XML_PARSE_NODICT | XML_PARSE_NOENT);
	doc = xmlCtxtReadFile(ctx, path.c_str(), "utf-8", XML_PARSE_NONET|XML_PARSE_NOCDATA);
	if(doc)
	{
		xmlFreeDoc(doc);
	}
	xmlFreeParserCtxt(ctx);
	return true;
}

/* Add a child object to this one; the child becomes owned by this object, and
 * will be destroyed by this object's destructor.
 */
void
Loadable::addObject(Loadable *child)
{
	child->m_owner = this;
	m_objects.push_back(child);
}

/* Invoked by the SAX startElementNS handler whenever an opening tag is
 * encountered in the XML document. Unless we're ignoring part of the DOM
 * tree (m_skip), startElement() calls factory() to create a new 
 * LoadableObject then calls the current tree leaf's add() method to add the
 * new object as a child (or sets the new object as m_root if there isn't a
 * tree yet), then sets the current tree leaf to the new object.
 */
void
Loadable::startElement(const xmlChar *localname, const xmlChar *prefix, const xmlChar *URI, int nb_namespaces, const xmlChar **namespaces, int nb_attributes, int nb_defaulted, const xmlChar **attributes)
{
	LoadableObject *obj;
	Ogre::String name((const char *) localname);
	Ogre::String ns(URI ? (const char *) URI : "");
	Ogre::String qname(ns + name);
	size_t i, c;
	AttrList attrs;
	
	if(m_skip)
	{
		/* We're already skipping the parent of this element, so ensure we
		 * skip this element (and its children) too.
		 */
		m_skip++;
		return;
	}
	for(i = c = 0; c < nb_attributes; c++)
	{
		Ogre::String ans(attributes[i + 2] ? (const char *) attributes[i + 2] : "");
		Ogre::String aname((const char *) attributes[i]);
		Ogre::String attr(ans + aname);
		Ogre::String value((const char *) attributes[i + 3], (size_t) (attributes[i+4] - attributes[i+3]));
		attrs.push_back(std::make_pair(attr, value));
		i += 5;
	}
	
	obj = factory(qname, attrs);
	if(!obj)
	{
		m_skip++;
		return;
	}
	if(m_cur)
	{
		if(!m_cur->addChild(obj))
		{
			delete obj;
			m_skip++;
		}
		m_cur = obj;
	}
	else
	{
		m_root = m_cur = obj;
	}
}

/* Invoked by the SAX startElementNS handler whenever a closing tag is
 * encountered in the XML document. If we're currently skipping part of the
 * tree, the skip depth (m_skip), otherwise, we adjust the current tree leaf
 * to point to its own parent.
 */
void
Loadable::endElement(const xmlChar *localname, const xmlChar *prefix, const xmlChar *URI)
{
	if(m_skip)
	{
		m_skip--;
	}
	else if(m_cur)
	{
		m_cur->didFinishLoading();
		m_cur = m_cur->parent();
	}
}

/* SAX callbacks, which simply invoke the equivalent methods on the Loadable
 * instance (which is passed as the ctx pointer).
 */
void
Loadable::sax_startElement(void *ctx, const xmlChar *localname, const xmlChar *prefix, const xmlChar *URI, int nb_namespaces, const xmlChar **namespaces, int nb_attributes, int nb_defaulted, const xmlChar **attributes)
{
	(reinterpret_cast<Loadable *>(ctx))->startElement(localname, prefix, URI, nb_namespaces, namespaces, nb_attributes, nb_defaulted, attributes);
}

void
Loadable::sax_endElement(void *ctx, const xmlChar *localname, const xmlChar *prefix, const xmlChar *URI)
{
	(reinterpret_cast<Loadable *>(ctx))->endElement(localname, prefix, URI);
}




/* LoadableObject is the base class used to encapsulate XML elements as
 * they're loaded from asset descriptions.
 *
 * Specialisations of LoadableObject deal with different kinds of element
 * in different contexts.
 *
 * Note that specifying a parent in the construction call is NOT the equivalent
 * of calling parent->add(Childself): that step must still be explicitly taken
 * (as it is by Loadable::startElement()) -- the parent instance, if any, is
 * supplied only as a hint.
 */

LoadableObject::LoadableObject(const LoadableObject &object):
	m_owner(NULL),
	m_name(""),
	m_attrs(),
	m_discardable(true)
{
	m_first = m_last = m_prev = m_next = m_parent = NULL;
	m_loaded = false;
	if(!object.complete())
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: cannot duplicate a loadable object instance which is incomplete");
		return;
	}
	if(object.m_discardable)
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: cannot duplicate a loadable object instance which is discardable");
		return;	
	}
	m_name = object.m_name;
	m_attrs = object.m_attrs;
	m_discardable = object.m_discardable;
	m_loaded = object.m_loaded;
	/* TODO: duplicate children */
}


LoadableObject::LoadableObject(Loadable *owner, LoadableObject *parent, Ogre::String name, AttrList &attrs):
	m_owner(owner),
	m_name(name),
	m_attrs(attrs),
	m_discardable(false)
{
	m_first = m_last = m_prev = m_next = m_parent = NULL;
	m_loaded = false;
}

LoadableObject::~LoadableObject()
{
	LoadableObject *p, *n;
	
	/* It's the parent's responsibility to detach us from the linked lists */
	for(p = m_first; p;)
	{
		n = p->m_next;
		delete p;
		p = n;
	}
}

LoadableObject *
LoadableObject::clone(void)
{
	return new LoadableObject(*this);
}

Ogre::String
LoadableObject::name(void) const
{
	return m_name;
}

LoadableObject *
LoadableObject::parent(void) const
{
	return m_parent;
}


/* Invoked by Loadable::complete() to check whether the asset has a complete
 * (useable) definition. This default implementation simply recurses the
 * children within the tree.
 */
bool
LoadableObject::complete(void) const
{
	LoadableObject *p;
	
	if(!m_loaded)
	{
		return false;
	}
	for(p = m_first; p; p = p->m_next)
	{
		if(!p->complete())
		{
			return false;
		}
	}
	return true;
}

/* Add a newly-created LoadableObject as a child of this one in the tree.
 * Invoked by Loadable::startElement() after a new LoadableObject is returned
 * by Loadable::factory().
 */
bool
LoadableObject::addChild(LoadableObject *child)
{
	child->m_parent = this;
	if(m_last)
	{
		m_last->m_next = child;
		child->m_prev = m_last;
	}
	else
	{
		m_first = child;
	}
	m_last = child;
	return true;
}

/* Invoked by Loadable::endElement() once the XML element that this
 * LoadableObject represents has been completely parsed (i.e., all of the
 * children in the tree are known to be present).
 * This default implementation simply notifies any children which haven't
 * already been notified (which will only happen if they were created through
 * some means other than as a result of the XML parsing callbacks).
 */
void
LoadableObject::didFinishLoading()
{
	LoadableObject *p;
	
	m_loaded = true;
	for(p = m_first; p; p = p->m_next)
	{
		if(!p->m_loaded)
		{
			p->didFinishLoading();
		}
	}
}

/* Invoked by Loadable::addResources() to perform any necessary asset data
 * loading; for example, a scene object might load meshes, materials and
 * textures ready for use.
 * This default implementation simply recurses the call down the tree.
 *
 * Scene objects should NOT be created by this method.
 */
bool
LoadableObject::addResources(Ogre::String group)
{
	LoadableObject *p;
	
	/* By default, invoke addResources() on each of the children */
	for(p = m_first; p; p = p->m_next)
	{
		p->addResources(group);
	}
	return true;
}

/* Destroy any child LoadableObjects marked as discardable */
void
LoadableObject::discard(void)
{
	LoadableObject *p;
	
	m_attrs.clear();
	for(p = m_first; p; p = p->m_next)
	{
		if(p->m_discardable)
		{
			if(p->m_prev)
			{
				p->m_prev->m_next = p->m_next;
			}
			else
			{
				this->m_first = p->m_next;
			}
			if(p->m_next)
			{
				p->m_next->m_prev = p->m_prev;
			}
			else
			{
				this->m_last = p->m_prev;
			}
			p->m_prev = NULL;
			p->m_next = NULL;
			delete p;
		}
	}
}

/* Utility method to parse a colour value specified in the attribute list */
Ogre::ColourValue
LoadableObject::parseColourValue(AttrList &attrs)
{
	double red = 0, green = 0, blue = 0, alpha = 1;
	AttrListIterator it;
	
	for(it = m_attrs.begin(); it != m_attrs.end(); it++)
	{
		Attr p = *it;
		
		if(!p.first.compare("r"))
		{
			red = atof(p.second.c_str());
		}
		else if(!p.first.compare("g"))
		{
			green = atof(p.second.c_str());
		}
		else if(!p.first.compare("b"))
		{
			blue = atof(p.second.c_str());
		}
		else if(!p.first.compare("a"))
		{
			alpha = atof(p.second.c_str());
		}
	}
	return Ogre::ColourValue(red, green, blue, alpha);
}

/* Utility method to parse a set of X/Y/Z co-ordinates specified in the
 * attribute list.
 */
Ogre::Vector3
LoadableObject::parseXYZ(AttrList &attrs, double x, double y, double z)
{
	AttrListIterator it;
	
	for(it = m_attrs.begin(); it != m_attrs.end(); it++)
	{
		Attr p = *it;
		if(!p.first.compare("x"))
		{
			x = atof(p.second.c_str());
		}
		else if(!p.first.compare("y"))
		{
			y = atof(p.second.c_str());
		}
		else if(!p.first.compare("z"))
		{
			z = atof(p.second.c_str());
		}
	}
	return Ogre::Vector3(x, y, z);
}
