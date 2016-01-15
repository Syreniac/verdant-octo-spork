#include "configuration.h"

int getConfiguredInt(ConfigurationData *configurationData, char *name){
  int i;
  ConfigurationFlag *flag = configurationData->flags;
  while(flag != NULL){
    if(strcmp(flag->name,name)){
      if(flag->type == CFT_INT){
        sscanf(flag->data,"%d",&i);
        return i;
      }
      else{
        assert(flag->type == CFT_INT);
      }
    }
    flag = flag->next;
  }
  assert(flag!=NULL);
  return 0;
}

ConfigurationFlag *makeConfigurationFlag(char *data){
  ConfigurationFlag *cf = calloc(1,sizeof(ConfigurationFlag));
  int equalsPosition = 0;
  int len = strlen(data);
  printf("making ");
  while(data[equalsPosition] != '='){
    if(equalsPosition >= len){
      return NULL;
    }
    equalsPosition++;
  }
  /* account for formatting looking much nicer with the space after equals */
  if(data[equalsPosition+1] == ' '){
    equalsPosition++;
  }
  switch(data[0]){
    case 'i':
      printf("CF_INT ");
      cf->type = CFT_INT;
      break;
    case 'd':
    case 'f':
      printf("CF_DOUBLE ");
      cf->type = CFT_DOUBLE;
      break;
    case 's':
      printf("CF_STRING ");
      cf->type = CFT_STRING;
      break;
    default:
      return NULL;
  }
  cf->name = calloc(equalsPosition,1);
  if(data[equalsPosition-2] == ' '){
    strncpy(cf->name,&data[1],equalsPosition - 3);
  }
  else{
    strncpy(cf->name,&data[1],equalsPosition - 1);
  }
  printf("called \'%s\' ",cf->name);
  cf->data = calloc(strlen(&data[equalsPosition])+1,1);
  strcpy(cf->data,&data[equalsPosition+1]);
  printf("with data \'%s\'\n",cf->data);
  return cf;
}

void initConfigurationData(ConfigurationData *configurationData){
  FILE *f;
  char *f2s;
  int i,previousIndex,i2;
  int previousPreviousIndex = 0;
  f = fopen("config.txt","r");
  assert(f!=NULL);
  f2s = fileToString(f);
  fclose(f);
  i = strlen(f2s);
  i2 = strlen(f2s);
  previousIndex = 0;


  while(i >= 0){
      if(f2s[i] == '\n' || f2s[i] == '\r'){
        f2s[i] = '\0';
      }
      i--;
  }
  i = 0;
  while(i <= i2){
      if(f2s[i] == '\0' || f2s[i] == EOF){
        free(makeConfigurationFlag(&f2s[previousIndex]));
        previousIndex = i+1;
      }
      i++;
  }

  free(f2s);
}
