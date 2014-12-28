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

#ifndef JYUZAU_LOADABLE_HH_
# define JYUZAU_LOADABLE_HH_           1

# include <libxml/parser.h>

namespace Jyuzau
{
	typedef std::pair<Ogre::String, Ogre::String> Attr;
	typedef std::vector<Attr> AttrList;
	typedef AttrList::iterator AttrListIterator;
	
	class LoadableObject;
	
	/* The Loadable class represents different kinds of assets which can be
	 * loaded from disk: scenes, props, actors, etc.
	 *
	 * It manages XML parsing, creation of the appropriate LoadableObject
	 * descendant instances, and provides the hooks for interfacing with
	 * OGRE's resource manager.
	 */
	class Loadable
	{
	public:
		Loadable(Ogre::String name, Ogre::String kind, bool subdir);
		~Loadable();
		
		virtual bool load(void);
		
		virtual LoadableObject *root(void);
		
		/* SAX callbacks */
		static void sax_startElement(void *ctx, const xmlChar *localname, const xmlChar *prefix, const xmlChar *URI, int nb_namespaces, const xmlChar **namespaces, int nb_attributes, int nb_defaulted, const xmlChar **attributes);
		static void sax_endElement(void *ctx, const xmlChar *localname, const xmlChar *prefix, const xmlChar *URI);

		/* This is public only because one cannot make all descendants of
		 * a class a friend
		 */
		virtual void add(Loadable *child);
	protected:
		Ogre::String m_name, m_kind, m_path, m_container, m_group;
		int m_skip;
		bool m_loaded, m_load_status;
		LoadableObject *m_root, *m_cur;
		Loadable *m_owner;
		std::vector<Loadable *> m_objects;
		
		virtual bool loadDocument(Ogre::String path);
		
		virtual void loaded(void);
		virtual bool addResources(Ogre::String group);
		
		virtual LoadableObject *factory(Ogre::String name, AttrList &attrs);
		
		virtual void startElement(const xmlChar *localname, const xmlChar *prefix, const xmlChar *URI, int nb_namespaces, const xmlChar **namespaces, int nb_attributes, int nb_defaulted, const xmlChar **attributes);
		virtual void endElement(const xmlChar *localname, const xmlChar *prefix, const xmlChar *URI);
	};
	
	/* LoadableObject is the base class used to encapsulate XML elements as
	 * they're loaded from asset descriptions.
	 *
	 * Specialisations of LoadableObject deal with different kinds of element
	 * in different contexts.
	 */
	class LoadableObject
	{
		friend class Loadable;
		
	public:
		LoadableObject(Loadable *owner, Ogre::String name, AttrList &attrs);
		virtual ~LoadableObject();
	
		virtual Ogre::String name(void);
		virtual LoadableObject *parent(void);
	protected:
		Loadable *m_owner;
		LoadableObject *m_parent, *m_first, *m_last, *m_prev, *m_next;
		Ogre::String m_name;
		bool m_loaded;
		AttrList m_attrs;
		
		virtual bool add(LoadableObject *child);
		virtual void loaded(void);
		virtual bool complete(void);
		virtual bool addResources(Ogre::String group);
	};
};

#endif /*!JYUZAU_LOADABLE_HH_*/