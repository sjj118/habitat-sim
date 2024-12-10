// Copyright (c) Meta Platforms, Inc. and its affiliates.
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include <cstdint>
#include <fstream>
#include <memory>
#include <vector>

#include "esp/assets/MeshData.h"
#include "esp/core/Esp.h"
#include "esp/scene/SemanticScene.h"
#include "esp/sensor/Sensor.h"

#ifdef ESP_BUILD_WITH_AUDIO
#include "rlr-audio-propagation/RLRAudioPropagationPkg/headers/RLRAudioPropagation.h"
#else
#include "esp/sensor/AudioSensorStubs.h"
#endif  // ESP_BUILD_WITH_AUDIO

#ifndef ESP_SENSOR_AUDIOSENSOR_H_
#define ESP_SENSOR_AUDIOSENSOR_H_

namespace esp {
namespace sensor {

struct AudioSensorSpec : public SensorSpec {
 public:
  AudioSensorSpec();
  void sanityCheck() const override;
  bool isVisualSensorSpec() const override { return false; }

#ifdef ESP_BUILD_WITH_AUDIO
 public:
  // Data
  RLRA_ContextConfiguration acousticsConfig_;
  RLRA_ChannelLayout channelLayout_;
  bool enableMaterials_ = true;
#endif  // ESP_BUILD_WITH_AUDIO

 public:
  ESP_SMART_POINTERS(AudioSensorSpec)

 private:
  const std::string logHeader_ = "[Audio] ";
};

class AudioSensor : public Sensor {
 public:
  // Constructor
  explicit AudioSensor(scene::SceneNode& node, AudioSensorSpec::ptr spec);
  // Destructor
  ~AudioSensor() override;

  /**
   * @brief Return that this is not a visual sensor
   */
  bool isVisualSensor() const override { return false; }

  // ------- functions to implement the audio sensor ------

#ifdef ESP_BUILD_WITH_AUDIO
  /**
   * @brief Clear the audio simulator object
   * */
  void reset();

  /**
   * @brief Set the audio source transform
   * @param sourcePos = vec3 source position
   * */
  void setAudioSourceTransform(const Magnum::Vector3& sourcePos);

  /**
   * @brief Set the audio listener position and orientation
   * @param agentPos = vec3 agent position
   * @param agentRotQuat = vec4 agent rotation quaternion
   * */
  void setAudioListenerTransform(const Magnum::Vector3& agentPos,
                                 const Magnum::Vector4& agentRotQuat);

  /**
   * @brief Run the audio simulation. This will run the RLRAudioPropagation code
   * to get the impulse response
   * */
  void runSimulation(sim::Simulator& sim);

  /**
   * @brief Set the audio materials database from a json file
   * */
  void setAudioMaterialsJSON(const std::string& jsonPath);

  /**
   * @brief Set the HRTF used for the audio listener, in SOFA (.sofa) format.
   * */
  void setListenerHRTF(const std::string& hrtfPath);

  /**
   * @brief Return the impulse response from the most recent simulation.
   * */
  const std::vector<std::vector<float>>& getIR();

  /**
   * @brief Write the impulse response as a .wav file.
   * */
  bool writeIRWave(const std::string& wavPath);

  /**
   * @brief Return a value in the range [0,1] indicating the
   * fraction of rays that hit something on the previous simulation.
   * */
  float getRayEfficiency() const;

  /**
   * @brief Write the entire audio scene as an OBJ mesh at the
   * specified path, for debugging purposes.
   * */
  bool writeSceneMeshOBJ(const std::string& objPath);

  /**
   * @brief Return whether the source is currently visible to the listener.
   * This shouldn't be called until after calling runSimulation() at least once.
   * */
  bool sourceIsVisible() const;
#endif  // ESP_BUILD_WITH_AUDIO

  // ------ Sensor class overrides ------
 public:
  bool getObservation(sim::Simulator&, Observation& obs) override;
  bool getObservationSpace(ObservationSpace& obsSpace) override;

 private:
  bool displayObservation(sim::Simulator&) override;

#ifdef ESP_BUILD_WITH_AUDIO
 private:
  /**
   * @brief Create the audio simulator object
   * */
  void createAudioSimulator();

  /**
   * @brief Load the semantic mesh for the Simulator object
   * */
  void loadSemanticMesh(sim::Simulator& sim);

  /**
   * @brief Load the non-sematic mesh for the Simulator object
   * */
  void loadMesh(sim::Simulator& sim);
#endif  // ESP_BUILD_WITH_AUDIO

 private:
  AudioSensorSpec::ptr audioSensorSpec_ =
      std::dynamic_pointer_cast<AudioSensorSpec>(spec_);

  const std::string logHeader_ = "[Audio] ";

#ifdef ESP_BUILD_WITH_AUDIO
  RLRA_Context context = nullptr;

  //! audio materials json path
  std::string audioMaterialsJSON_;

  bool audioMaterialsJsonSet_ = false;
  bool newInitialization_ = false;

  std::vector<std::vector<float>> impulseResponse_;

  Magnum::Vector3 sourcePosition_;
  Magnum::Vector3 listenerPosition_;
#endif  // ESP_BUILD_WITH_AUDIO

 public:
  ESP_SMART_POINTERS(AudioSensor)
};  // class AudioSensor

}  // namespace sensor
}  // namespace esp

#endif  // ESP_SENSOR_AUDIOSENSOR_H_
