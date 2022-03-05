#pragma once

#include <string>
#include "core/module.h"
#include <mutex>
#include <vector>
#include "dll_export.h"
#include "nlohmann/json.hpp"
#include <optional>

struct PipelineModule
{
    std::string module_name;
    nlohmann::json parameters;
    std::string input_override;
};

struct PipelineStep
{
    std::string level_name;
    std::vector<PipelineModule> modules;
};

struct Pipeline
{
    std::string name;
    std::string readable_name;
    std::string category;
    bool live;
    std::vector<float> frequencies;
    long default_samplerate;
    std::string default_baseband_type;
    std::vector<std::pair<int, int>> live_cfg;

    nlohmann::json editable_parameters;

    std::vector<PipelineStep> steps;
    void run(std::string input_file,
             std::string output_directory,
             nlohmann::json parameters,
             std::string input_level,
             bool ui = false,
             std::shared_ptr<std::vector<std::shared_ptr<ProcessingModule>>> uiCallList = nullptr,
             std::shared_ptr<std::mutex> uiCallListMutex = nullptr);

    nlohmann::json prepareParameters(nlohmann::json &module_params, nlohmann::json &pipeline_params);
};

SATDUMP_DLL extern std::vector<Pipeline> pipelines;
SATDUMP_DLL extern std::vector<std::string> pipeline_categories;

void loadPipelines(std::string filepath, std::vector<Pipeline> &pipelines = pipelines, std::map<std::string, std::function<std::shared_ptr<ProcessingModule>(std::string, std::string, nlohmann::json)>> &modules_registry = modules_registry);
std::vector<Pipeline> getPipelinesInCategory(std::string category);
Pipeline getPipelineInCategoryFromId(std::string category, int id);
std::optional<Pipeline> getPipelineFromName(std::string downlink_pipeline);