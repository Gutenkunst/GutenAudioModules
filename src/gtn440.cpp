#include "Guten.hpp"
#include "dsp/digital.hpp"

#define NUM_NOTES 12

struct gtn440 : Module {
    enum ParamIds {
        PITCH_PARAM,
        NUM_PARAMS = PITCH_PARAM + NUM_NOTES
    };
    enum InputIds {
        PITCH_INPUT,
        NUM_INPUTS
    };
    enum OutputIds {
        PITCH_OUTPUT,
        NUM_OUTPUTS
    };
    enum LightIds {
        BLINK_LIGHT,
        NUM_LIGHTS = BLINK_LIGHT + NUM_NOTES
    };

    bool state[NUM_NOTES];
    SchmittTrigger muteTrigger[NUM_NOTES];

    gtn440() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
    void step() override;

    // For more advanced Module features, read Rack's engine.hpp header file
    // - toJson, fromJson: serialization of internal data
    // - onSampleRateChange: event triggered by a change of sample rate
    // - onReset, onRandomize, onCreate, onDelete: implements special behavior when user clicks these from the context menu
    json_t *toJson() override {
        json_t *rootJ = json_object();
        // states
        json_t *statesJ = json_array();
        for (int i = 0; i < NUM_NOTES; i++) {
            json_t *stateJ = json_boolean(state[i]);
            json_array_append_new(statesJ, stateJ);
        }
        json_object_set_new(rootJ, "states", statesJ);
        return rootJ;
    }
    void fromJson(json_t *rootJ) override {
        // states
        json_t *statesJ = json_object_get(rootJ, "states");
        if (statesJ) {
            for (int i = 0; i < NUM_NOTES; i++) {
                json_t *stateJ = json_array_get(statesJ, i);
                if (stateJ)
                    state[i] = json_boolean_value(stateJ);
            }
        }
    }
};


void gtn440::step() {
    float pitch = inputs[PITCH_INPUT].value;
    float octave = floor(pitch);
    float semitone = (pitch - octave) * 12;

    int quantized = 0;
    float minDistance = 12;
    for (int i = 0; i < NUM_NOTES; i++) {
        if (muteTrigger[i].process(params[PITCH_PARAM + i].value))
            state[i] ^= true;

        if (state[i])
        {
            float distance = abs(i - semitone);
            if (distance < minDistance)
            {
                minDistance = distance;
                quantized = i;
            }
        }
        lights[BLINK_LIGHT + i].setBrightness(state[i] ? 0.9f : 0.0f);
    }

    pitch = octave + quantized / 12.f;

    outputs[PITCH_OUTPUT].value = pitch;
}


struct gtn440Widget : ModuleWidget {
    gtn440Widget(gtn440 *module) : ModuleWidget(module) {
        setPanel(SVG::load(assetPlugin(plugin, "res/gtn440.svg")));

        addChild(Widget::create<Hole>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(Widget::create<Hole2>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        addParam(ParamWidget::create<LEDBezel>(mm2px(Vec(6.57, 48.165)), module, gtn440::PITCH_PARAM + 0, 0.0f, 1.0f, 0.0f));
        addParam(ParamWidget::create<LEDBezel>(mm2px(Vec(18.214, 53.164)), module, gtn440::PITCH_PARAM + 1, 0.0f, 1.0f, 0.0f));
        addParam(ParamWidget::create<LEDBezel>(mm2px(Vec(6.57, 58.164)), module, gtn440::PITCH_PARAM + 2, 0.0f, 1.0f, 0.0f));
        addParam(ParamWidget::create<LEDBezel>(mm2px(Vec(18.214, 63.165)), module, gtn440::PITCH_PARAM + 3, 0.0f, 1.0f, 0.0f));
        addParam(ParamWidget::create<LEDBezel>(mm2px(Vec(6.57, 68.164)), module, gtn440::PITCH_PARAM + 4, 0.0f, 1.0f, 0.0f));
        addParam(ParamWidget::create<LEDBezel>(mm2px(Vec(6.57, 78.165)), module, gtn440::PITCH_PARAM + 5, 0.0f, 1.0f, 0.0f));
        addParam(ParamWidget::create<LEDBezel>(mm2px(Vec(18.214, 83.164)), module, gtn440::PITCH_PARAM + 6, 0.0f, 1.0f, 0.0f));
        addParam(ParamWidget::create<LEDBezel>(mm2px(Vec(6.57, 88.164)), module, gtn440::PITCH_PARAM + 7, 0.0f, 1.0f, 0.0f));
        addParam(ParamWidget::create<LEDBezel>(mm2px(Vec(18.214, 93.165)), module, gtn440::PITCH_PARAM + 8, 0.0f, 1.0f, 0.0f));
        addParam(ParamWidget::create<LEDBezel>(mm2px(Vec(6.57, 98.165)), module, gtn440::PITCH_PARAM + 9, 0.0f, 1.0f, 0.0f));
        addParam(ParamWidget::create<LEDBezel>(mm2px(Vec(18.214, 103.165)), module, gtn440::PITCH_PARAM + 10, 0.0f, 1.0f, 0.0f));
        addParam(ParamWidget::create<LEDBezel>(mm2px(Vec(6.57, 108.166)), module, gtn440::PITCH_PARAM + 11, 0.0f, 1.0f, 0.0f));

        addInput(Port::create<PJ301MPort>(mm2px(Vec(4.214, 17.81)), Port::INPUT, module, gtn440::PITCH_INPUT));

        addOutput(Port::create<PJ301MPort>(mm2px(Vec(20.214, 17.81)), Port::OUTPUT, module, gtn440::PITCH_OUTPUT));

        addChild(ModuleLightWidget::create<LEDBezelLight<GreenLight>>(mm2px(Vec(7.32, 48.915)), module, gtn440::BLINK_LIGHT + 0));
        addChild(ModuleLightWidget::create<LEDBezelLight<GreenLight>>(mm2px(Vec(18.964, 53.916)), module, gtn440::BLINK_LIGHT + 1));
        addChild(ModuleLightWidget::create<LEDBezelLight<GreenLight>>(mm2px(Vec(7.32, 58.915)), module, gtn440::BLINK_LIGHT + 2));
        addChild(ModuleLightWidget::create<LEDBezelLight<GreenLight>>(mm2px(Vec(18.964, 63.915)), module, gtn440::BLINK_LIGHT + 3));
        addChild(ModuleLightWidget::create<LEDBezelLight<GreenLight>>(mm2px(Vec(7.32, 68.916)), module, gtn440::BLINK_LIGHT + 4));
        addChild(ModuleLightWidget::create<LEDBezelLight<GreenLight>>(mm2px(Vec(7.32, 78.916)), module, gtn440::BLINK_LIGHT + 5));
        addChild(ModuleLightWidget::create<LEDBezelLight<GreenLight>>(mm2px(Vec(18.964, 83.915)), module, gtn440::BLINK_LIGHT + 6));
        addChild(ModuleLightWidget::create<LEDBezelLight<GreenLight>>(mm2px(Vec(7.32, 88.916)), module, gtn440::BLINK_LIGHT + 7));
        addChild(ModuleLightWidget::create<LEDBezelLight<GreenLight>>(mm2px(Vec(18.964, 93.915)), module, gtn440::BLINK_LIGHT + 8));
        addChild(ModuleLightWidget::create<LEDBezelLight<GreenLight>>(mm2px(Vec(7.32, 98.915)), module, gtn440::BLINK_LIGHT + 9));
        addChild(ModuleLightWidget::create<LEDBezelLight<GreenLight>>(mm2px(Vec(18.964, 103.915)), module, gtn440::BLINK_LIGHT + 10));
        addChild(ModuleLightWidget::create<LEDBezelLight<GreenLight>>(mm2px(Vec(7.32, 108.915)), module, gtn440::BLINK_LIGHT + 11));
    }
};


// Specify the Module and ModuleWidget subclass, human-readable
// author name for categorization per plugin, module slug (should never
// change), human-readable module name, and any number of tags
// (found in `include/tags.hpp`) separated by commas.
Model *modelgtn440 = Model::create<gtn440, gtn440Widget>("GutenAudioModules", "gtn440", "440 CV Quantizer", QUANTIZER_TAG);
