#ifndef __DATABASE_RECORD_H__
#define __DATABASE_RECORD_H__

#define FIELD_DELIMITER ':'
#define ROW_DELIMITER   '|'; 
#define MAX_RECORD_LEN  100; 

class DatabaseRecord
{
  private: 
    String _fullLine; 
    String _sceneNumber; 
    String _sceneName; 
    int _field1Len = 0; 
    int _field2Len = 0; 
    
  public: 
    DatabaseRecord(int sceneNumber, char* sceneName); 
    DatabaseRecord(char* data); 
    
    String* getSceneNumber(); 
    String* getSceneName(); 
    void setSceneNumber(int value); 
    void setSceneNumber(char* value); 
    void setSceneName(char* value); 
    String* toString(); 
    
  private: 
    void parse(char* data); 
};


DatabaseRecord::DatabaseRecord(int sceneNumber, char* sceneName)
{
  this->setSceneNumber(sceneNumber); 
  this->setSceneName(sceneName); 
}

DatabaseRecord::DatabaseRecord(char* data)
{
  this->parse(data); 
}

String* DatabaseRecord::getSceneNumber()
{
  this->_sceneNumber = this->_fullLine.substring(0, this->_field1Len); 
  return &this->_sceneNumber; 
}

String* DatabaseRecord::getSceneName()
{
  int startIndex = (this->_field1Len+1); 
  if (startIndex > 1)
  {
    this->_sceneName = this->_fullLine.substring(startIndex, startIndex+this->_field2Len); 
  }
  else
    this->_sceneName = ""; 
    
  return &this->_sceneName; 
}

void DatabaseRecord::setSceneNumber(int value)
{
  this->_sceneNumber = "" + value; 
  this->_fullLine = this->getSceneNumber() + FIELD_DELIMITER + this->getSceneName() + ROW_DELIMITER; 
}

void DatabaseRecord::setSceneNumber(char* value)
{
  this->_sceneNumber = value; 
  this->_fullLine = this->getSceneNumber() + FIELD_DELIMITER + this->getSceneName() + ROW_DELIMITER; 
}

void DatabaseRecord::setSceneName(char* value)
{
  this->_sceneName = value; 
  this->_fullLine = this->getSceneNumber() + FIELD_DELIMITER + this->getSceneName() + ROW_DELIMITER; 
}

String* DatabaseRecord::toString()
{
  return this->_fullLine; 
}

String* DatabaseRecord::parse(char* data)
{
  this->_field1Len = 0; 
  this->_field2Len = 0; 
  int fieldNum = 0; 
  
  for(int n=0; n<MAX_RECORD_LEN; n++)
  {
    char c = data[n]; 
    if (c == ROW_DELIMITER || c == 0)
      break;
        
    if (fieldNum == 0)
    {
      if (c == FIELD_DELIMITER)
        fieldNum++; 
      else
        this->_field1Len++; 
    }
    else
    {
      this->_fieldLen2++; 
    }
  }
}

#endif

