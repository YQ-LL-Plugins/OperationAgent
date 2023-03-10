#include "Config.h"
#include <Nlohmann/json.hpp>
#include <llapi/LoggerAPI.h>
#include <filesystem>

#pragma region Config

extern Logger logger;

// Write config value to json
#define SerializeVaule(var) json[#var] = Config::var
// Write config enum value to json
#define SerializeEnumVaule(var) json[#var] = magic_enum::enum_name(Config::var)

// Read config value from json
#define DeserializeVaule(var)                                                         \
    if (json.find(#var) != json.end())                                                \
    {                                                                                 \
        Config::var = json.value(#var, Config::var);                                  \
    }                                                                                 \
    else                                                                              \
    {                                                                                 \
        logger.info("Missing config {}, use default value ", #var /*, Config::var*/); \
        needUpdate = true;                                                            \
    }

// Read config enum value from json
#define DeserializeEnumVaule(var)                                            \
    if (json.find(#var) != json.end())                                       \
    {                                                                        \
        auto svar = magic_enum::enum_name(Config::var);                      \
        svar = json.value(#var, svar);                                       \
        auto enumValue = magic_enum::enum_cast<decltype(Config::var)>(svar); \
        if (enumValue.has_value())                                           \
            Config::var = enumValue.value();                                 \
        else                                                                 \
        {                                                                    \
            logger.warn("Unsupported config value {}, use default value {}", \
                        svar, magic_enum::enum_name(Config::var));           \
            needUpdate = true;                                               \
        }                                                                    \
    }                                                                        \
    else                                                                     \
    {                                                                        \
        logger.warn("Missing config {}, use default value {}",               \
                    #var, magic_enum::enum_name(Config::var));               \
        needUpdate = true;                                                   \
    }

namespace Config
{

std::string serialize()
{
    nlohmann::json json;

    SerializeVaule(autoClean);
    SerializeVaule(cancelAfterSleep);
    SerializeVaule(cancelAfterRide);
    SerializeVaule(autoSleep);
    SerializeVaule(autoRideWhenJoin);
    SerializeVaule(useNewProjectMode);
    SerializeVaule(forProjectile);
    SerializeVaule(forAttack);
    SerializeVaule(forSleep);
    //SerializeVaule(forMove);
    SerializeVaule(forRide);
    SerializeVaule(autoSwapAttack);

    return json.dump(4);
}

bool deserialize(std::string const& jsonStr)
{
    auto json = nlohmann::json::parse(jsonStr, nullptr, false, true);
    bool needUpdate = false;

    DeserializeVaule(autoClean);
    DeserializeVaule(cancelAfterSleep);
    DeserializeVaule(cancelAfterRide);
    DeserializeVaule(autoSleep);
    DeserializeVaule(autoRideWhenJoin);
    DeserializeVaule(useNewProjectMode);
    DeserializeVaule(forProjectile);
    DeserializeVaule(forAttack);
    DeserializeVaule(forSleep);
    //DeserializeVaule(forMove);
    DeserializeVaule(forRide);
    DeserializeVaule(autoSwapAttack);

    return !needUpdate;
}

bool saveConfig()
{
    return WriteAllFile(PLUGIN_CONFIG_PATH, serialize(), false);
}

bool initConfig()
{
    bool needToSave = false;
    auto jsonStr = ReadAllFile(PLUGIN_CONFIG_PATH);
    if (jsonStr.has_value())
    {
        try
        {
            needToSave = !deserialize(jsonStr.value());
        }
        catch (const std::exception&)
        {
            logger.error("Error in loading config file \"{}\", Use default config", PLUGIN_CONFIG_PATH);
            needToSave = true;
        }
    }
    else
    {
        logger.info("Config File \"{}\" Not Found, Use Default Config", PLUGIN_CONFIG_PATH);
        std::filesystem::create_directories(std::filesystem::path(PLUGIN_CONFIG_PATH).remove_filename());
        needToSave = true;
    }
    if (needToSave)
        needToSave = saveConfig();
    return !needToSave;
}

} // namespace Config

#pragma endregion

#if PLUGIN_VERSION_IS_BETA
void logBetaInfo()
{
    logger.warn("beta version, log info:");
    LOG_VAR(PLUGIN_NAME);
    if (ENABLE_LOG_FILE)
        LOG_VAR(PLUGIN_LOG_PATH);
    if (ENABLE_CONFIG)
        LOG_VAR(PLUGIN_CONFIG_PATH);

    //LOG_VAR(PLUGIN_AUTHOR);
    //LOG_VAR(PLUGIN_DISPLAY_NAME);
    //LOG_VAR(PLUGIN_INTRODUCTION);
    //LOG_VAR(PLUGIN_WEBSIDE);
    //LOG_VAR(PLUGIN_LICENCE);
    //LOG_VAR(PLUGIN_USAGE);

    //LOG_VAR(LOG_DIR);
    //LOG_VAR(PLUGINS_DIR);
    //LOG_VAR(PLUGIN_DIR);
    //LOG_VAR(PLUGIN_LOG_PATH);
    //LOG_VAR(PLUGIN_DATA_PATH);

    //LOG_VAR(PLUGIN_FILE_VERSION_STRING);
}
#endif // PLUGIN_VERSION_IS_BETA