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

/* The Loadable class represents different kinds of assets which can be
 * loaded from disk: scenes, props, actors, etc.
 *
 * It manages XML parsing, creation of the appropriate LoadableObject
 * descendant instances, and provides the hooks for interfacing with
 * OGRE's resource manager.
 */

Loadable::Loadable(Ogre::String name, Ogre::String kind, bool subdir):
	m_name(name),
	m_kind(kind),
	m_root(NULL),
	m_cur(NULL),
	m_objects(),
	m_owner(NULL)
{
	Ogre::String base;
	
	m_loaded = m_load_status = false;
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

bool
Loadable::load(void)
{
	if(m_loaded)
	{
		return m_load_status;
	}
	m_loaded = true;
	m_load_status = false;
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
		return false;
	}
	m_load_status = true;
	loaded();
	return true;
}

Ogre::String
Loadable::name(void)
{
	return m_name;
}

Ogre::String
Loadable::kind(void)
{
	return m_kind;
}

void
Loadable::loaded(void)
{
	if(!addResources(m_group))
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: failed to add resources from " + m_group);
		m_load_status = false;
		return;
	}
	Ogre::LogManager::getSingletonPtr()->logMessage("Jyuzau: Loaded " + m_kind + "::" + m_name + " from " + m_path);
}

bool
Loadable::addResources(Ogre::String group)
{
	if(m_root && !m_root->addResources(group))
	{
		return false;
	}
	return true;
}

LoadableObject *
Loadable::factory(Ogre::String name, AttrList &attrs)
{
	/* Default loadable object factory; simply returns a new instance
	 * of LoadableObject
	 */
	return new LoadableObject(this, m_root, name, attrs);
}

LoadableObject *
Loadable::root(void)
{
	return m_root;
}

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
Loadable::add(Loadable *child)
{
	child->m_owner = this;
	m_objects.push_back(child);
}

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
		if(!m_cur->add(obj))
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

void
Loadable::endElement(const xmlChar *localname, const xmlChar *prefix, const xmlChar *URI)
{
	if(m_skip)
	{
		m_skip--;
	}
	else if(m_cur)
	{
		m_cur->loaded();
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
 * Note that specifying a parent here is NOT the equivalent of calling
 * parent->add(self): that step must still be explicitly taken (as it is
 * by Loadable::startElement()).
 */

LoadableObject::LoadableObject(Loadable *owner, LoadableObject *parent, Ogre::String name, AttrList &attrs):
	m_owner(owner),
	m_name(name),
	m_attrs(attrs)
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

Ogre::String
LoadableObject::name(void)
{
	return m_name;
}

bool
LoadableObject::complete(void)
{
	LoadableObject *p;
	
	for(p = m_first; p; p = p->m_next)
	{
		if(!p->complete())
		{
			return false;
		}
	}
	return true;
}

LoadableObject *
LoadableObject::parent(void)
{
	return m_parent;
}

bool
LoadableObject::add(LoadableObject *child)
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

void
LoadableObject::loaded()
{
	LoadableObject *p;
	
	m_loaded = true;
	for(p = m_first; p; p = p->m_next)
	{
		if(!p->m_loaded)
		{
			p->loaded();
		}
	}
}

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


