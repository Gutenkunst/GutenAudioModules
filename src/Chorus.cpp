#include "Guten.hpp"
#include "dsp/ringbuffer.hpp"


struct Chorus : Module {
	enum ParamIds {
        STEREO_PARAM,
        WET_PARAM,
        MOD_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		MONO_INPUT,
        MOD_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
        LEFT_OUTPUT,
        RIGHT_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		BLINK_LIGHT,
		NUM_LIGHTS
	};

	float phase = 0.0;
    float delayInSeconds = 0.025f;
    float sampleRate = engineGetSampleRate();
    RingBuffer<float, 16384> buffer{};

	Chorus() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override;

	// For more advanced Module features, read Rack's engine.hpp header file
	// - toJson, fromJson: serialization of internal data
	// - onSampleRateChange: event triggered by a change of sample rate
	// - onReset, onRandomize, onCreate, onDelete: implements special behavior when user clicks these from the context menu
    void onCreate() override
    {
        for (int i = (int)(sampleRate * delayInSeconds); i > 0; --i)
        {
            buffer.push(0);
        }
    }
};


void Chorus::step() {
    buffer.push(inputs[MONO_INPUT].value);
    buffer.shift();

    float stereo = params[STEREO_PARAM].value;
    float wet = params[WET_PARAM].value;
    float mod = params[MOD_PARAM].value;

    float deltaTime = engineGetSampleTime();

	float pitch = mod + inputs[MOD_INPUT].value;
	pitch = clamp(pitch, -4.0f, 4.0f);

	float freq = .8f * powf(2.0f, pitch);

	// Accumulate the phase
	phase += freq * deltaTime;
	if (phase >= 1.0f)
		phase -= 1.0f;

	// Compute the sine output
    float sineBetween0and1 = (sinf(2.0f * M_PI * phase) + 1.f) * .5f;
	float sine = sineBetween0and1 * 0.015f * sampleRate;
    float side = buffer.data[buffer.mask(buffer.start - sine)];

    outputs[LEFT_OUTPUT].value = inputs[MONO_INPUT].value * (1 - wet) + side * wet;

	lights[BLINK_LIGHT].value = sineBetween0and1;
}

struct ChorusWidget : ModuleWidget {
	ChorusWidget(Chorus *module) : ModuleWidget(module) {
		setPanel(SVG::load(assetPlugin(plugin, "res/Chorus.svg")));

        addChild(Widget::create<Schraube>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(Widget::create<Hole2>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(Widget::create<Hole2>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(Widget::create<Schraube>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        addParam(ParamWidget::create<Davies1900hBlackKnob>(Vec(28, 20), module, Chorus::STEREO_PARAM, 0.0, 1.0, 0.0));
        addParam(ParamWidget::create<Davies1900hBlackKnob>(Vec(28, 60), module, Chorus::WET_PARAM, 0.0, 1.0, 0.5));
        addParam(ParamWidget::create<Davies1900hBlackKnob>(Vec(28, 100), module, Chorus::MOD_PARAM, -3.0, 3.0, 0.0));

        addInput(Port::create<PJ301MPort>(Vec(33, 140), Port::INPUT, module, Chorus::MONO_INPUT));
        addInput(Port::create<PJ301MPort>(Vec(33, 180), Port::INPUT, module, Chorus::MOD_INPUT));

        addOutput(Port::create<PJ301MPort>(Vec(33, 220), Port::OUTPUT, module, Chorus::LEFT_OUTPUT));
        addOutput(Port::create<PJ301MPort>(Vec(33, 260), Port::OUTPUT, module, Chorus::RIGHT_OUTPUT));

		addChild(ModuleLightWidget::create<MediumLight<RedLight>>(Vec(41, 330), module, Chorus::BLINK_LIGHT));
	}
};


// Specify the Module and ModuleWidget subclass, human-readable
// author name for categorization per plugin, module slug (should never
// change), human-readable module name, and any number of tags
// (found in `include/tags.hpp`) separated by commas.
Model *modelChorus = Model::create<Chorus, ChorusWidget>("GutenAudioModules", "gtnChorus", "Chorus", CHORUS_TAG);
