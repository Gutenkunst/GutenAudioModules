#include "rack.hpp"


using namespace rack;

// Forward-declare the Plugin, defined in Template.cpp
extern Plugin *plugin;

// Forward-declare each Model, defined in each module source file
extern Model *modelgtn440;
extern Model *modelChorus;
extern Model *modelEnvelopeFollower;


struct Schraube : SVGScrew {
    Schraube() {
        sw->svg = SVG::load(assetPlugin(plugin, "res/Schraube.svg"));
        sw->wrap();
        box.size = sw->box.size;
    }
};
struct Hole : SVGScrew {
    Hole() {
        sw->svg = SVG::load(assetPlugin(plugin, "res/Hole.svg"));
        sw->wrap();
        box.size = sw->box.size;
    }
};
struct Hole2 : SVGScrew {
    Hole2() {
        sw->svg = SVG::load(assetPlugin(plugin, "res/Hole2.svg"));
        sw->wrap();
        box.size = sw->box.size;
    }
};