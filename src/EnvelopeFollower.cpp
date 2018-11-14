#include "Guten.hpp"

struct EnvelopeFollower : Module {
	enum ParamIds {
		AMOUNT_PARAM,
		OFFSET_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		AUDIO_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		ENVELOPE_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		BLINK_LIGHT,
		NUM_LIGHTS
	};

	float currentValue = 0.0;

	EnvelopeFollower() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override;

	// For more advanced Module features, read Rack's engine.hpp header file
	// - toJson, fromJson: serialization of internal data
	// - onSampleRateChange: event triggered by a change of sample rate
	// - onReset, onRandomize, onCreate, onDelete: implements special behavior when user clicks these from the context menu
};


void EnvelopeFollower::step() {

	float amount = params[AMOUNT_PARAM].value;
	float offset = params[OFFSET_PARAM].value;
	float input = inputs[AUDIO_INPUT].value;

	currentValue = currentValue * 0.9f + abs(input) * 0.1f;

	outputs[ENVELOPE_OUTPUT].value = offset + currentValue * amount;

	lights[BLINK_LIGHT].value = currentValue;
}


struct EnvelopeFollowerWidget : ModuleWidget {
	EnvelopeFollowerWidget(EnvelopeFollower *module) : ModuleWidget(module) {
		setPanel(SVG::load(assetPlugin(plugin, "res/EnvelopeFollower.svg")));

		addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(ParamWidget::create<Davies1900hBlackKnob>(Vec(box.size.x*.05f, 87), module, EnvelopeFollower::OFFSET_PARAM, -3.0, 3.0, 0.0));
		addParam(ParamWidget::create<Davies1900hBlackKnob>(Vec(box.size.x*.55f, 87), module, EnvelopeFollower::AMOUNT_PARAM, -3.0, 3.0, 0.0));
        
		addInput(Port::create<PJ301MPort>(Vec(33, 186), Port::INPUT, module, EnvelopeFollower::AUDIO_INPUT));

		addOutput(Port::create<PJ301MPort>(Vec(33, 275), Port::OUTPUT, module, EnvelopeFollower::ENVELOPE_OUTPUT));

		addChild(ModuleLightWidget::create<MediumLight<RedLight>>(Vec(41, 59), module, EnvelopeFollower::BLINK_LIGHT));
	}
};


// Specify the Module and ModuleWidget subclass, human-readable
// author name for categorization per plugin, module slug (should never
// change), human-readable module name, and any number of tags
// (found in `include/tags.hpp`) separated by commas.
Model *modelEnvelopeFollower = Model::create<EnvelopeFollower, EnvelopeFollowerWidget>("GutenAudioModules", "EnvelopeFollower", "Envelope Follower", ENVELOPE_FOLLOWER_TAG);
