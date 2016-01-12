#include "generic.h"
enum ConfigurationFlagType{
  CFT_STRING,
  CFT_INT,
  CFT_DOUBLE,
};
typedef enum ConfigurationFlagType ConfigurationFlagType;
typedef struct ConfigurationData ConfigurationData;
typedef struct ConfigurationFlag ConfigurationFlag;

struct ConfigurationFlag{
  ConfigurationFlagType type;
  ConfigurationFlag *next;
  char *data;
  char *name;
};

struct ConfigurationData{
  ConfigurationFlag *flags;
};

void initConfigurationData(ConfigurationData *configurationData);
ConfigurationFlag *makeConfigurationFlag(char *data);
