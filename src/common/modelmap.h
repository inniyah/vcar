#ifndef MODELMAP_H
#define MODELMAP_H

#include <string>
#include <map>

#include <plib/ssg.h>

class ssgEntity;

typedef std::map<std::string, ssgEntity*> map_t;

class ModelMap
{
  public:

    ModelMap(const std::string &pref) :
    prefix(pref)
    {
    }

    ~ModelMap()
    {
    }

    bool LowDetail(void) const
    {
      static bool low_detail = getenv("PLODE_LOW_DETAIL");
      return low_detail;
    }

    ssgEntity *Get(const std::string &name)
    {
      if (map.find(name) != map.end())
        return map[name];
      std::string p = prefix + "/models/";
      ssgTexturePath(p.c_str());
      std::string fname = p+name;
      map[name] = ssgLoad(fname.c_str());
      assert(map[name]);
      map[name]->ref();
      //MakeDisplayLists(map[name]);
      WorkAroundSpecularBugInLoader(name, map[name]);
      return map[name];
    }

    void Put(const std::string &name, ssgEntity *model)
    {
      map[name] = model;
    }

    // plib 1.8.4 has a bug in 3ds loader
    void WorkAroundSpecularBugInLoader(const std::string &name, ssgEntity *node, float fixed_shininess=8.0)
    {
      if (node->isAKindOf(ssgTypeBranch()))
      {
        ssgBranch *branch = dynamic_cast<ssgBranch*>(node);
        assert(branch);
        for (int i=0; i<branch->getNumKids(); i++)
          WorkAroundSpecularBugInLoader(name, branch->getKid(i), fixed_shininess);
      }
      if (node->isAKindOf(ssgTypeLeaf()))
      {
        ssgLeaf *leaf = dynamic_cast<ssgLeaf*>(node);
        assert(leaf);
        ssgState *state = leaf->getState();
        if (state)
        {
          ssgSimpleState *simplestate = dynamic_cast<ssgSimpleState*>(state);
          assert(simplestate);
          float *spec = simplestate->getMaterial(GL_SPECULAR);
          float  shin = simplestate->getShininess();
          if (shin == 0.0)
          {
            static bool invalid_shininess_found=false;
            if (spec[0] || spec[1] || spec[2])
            {
              if (!invalid_shininess_found)
                fprintf
                (
                  stderr,
                  "Your plib (version %d.%d.%d) has a bug in the 3ds loader. Workaround enabled.\n", 
                  PLIB_MAJOR_VERSION, PLIB_MINOR_VERSION, PLIB_TINY_VERSION
                );
              invalid_shininess_found = true;
              //fprintf(stderr,"Patching invalid shininess/specular combo in %s\n", name.c_str());
              simplestate->setShininess(fixed_shininess);
            }
          }
        }
      }
    }

  protected:
    const std::string &prefix;
    map_t map;
};

#endif
