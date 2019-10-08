/*

 SD - a slightly more friendly wrapper for fatfs. 
 

 Created by hongtai.liu 13 July 2019


 */
#include <Arduino.h>
#include <Seeed_FS.h>


namespace fs {

File::File(FIL f, const char *n) { //is a file.
  _file = new FIL(f);
  if (_file) {
     _dir = NULL;
     sprintf((char*)_name, "%s", n);
  }
}

File::File(DIR d, const char *n) {// is a directory
  _dir = new DIR(d);
  if (_dir) {
    _file = NULL;
    sprintf((char*)_name, "%s", n);
  }
}


File::File(void) {  
  _file = 0;
  _name[0] = 0;
  //Serial.print("Created empty file object");
}

// returns a pointer to the file name
char *File::name(void) {
  return _name;
}


// a directory is a special type of file
boolean File::isDirectory(void) {
  FRESULT ret = FR_OK;
  FILINFO v_fileinfo;
   if((ret = f_stat(_name, &v_fileinfo)) == FR_OK){
      if (v_fileinfo.fattrib & AM_DIR)
      {
        return true;
      }else{
        return false;
      }
   }else{
    return false;
   }
   return false;
}


size_t File::write(uint8_t val) {
  return write(&val, 1);
}

size_t File::write(const uint8_t *buf, size_t size) {
  UINT t;
  if (!_file) {
    return 0;
  }
  if(f_write(_file, buf, size, &t) == -1) // if success, return the number of bytes have written.
  {
    return 0;
  }
  else
    return t;
}

//return the peek value
int File::peek() { 
  if (! _file) 
    return 0;

  int c = read();
  if (c != -1) f_lseek(_file, f_tell(_file) - 1);
  return c;
}

// read a value 
int File::read() {
  uint8_t val;
  if (_file){
     return read(&val, 1) == 1 ? val : -1;
  }
  return -1;
}

long File::read(void *buf, uint32_t nbyte) {
  UINT t;
  if (!_file) {
    return 0;
  }
  if(f_read(_file, buf, nbyte, &t) == -1)
    return 0;
  else
  return t;
}

int File::available() {
  if (! _file) return 0;

   return !f_eof(_file);
 
}

void File::flush() {
  if (_file)
    f_sync(_file);
}

boolean File::seek(uint32_t pos) {
  if (! _file) return false;

  return f_lseek(_file, pos);
}

uint32_t File::position() {
  if (! _file) return -1;
  return f_tell(_file);
}

uint32_t File::size() {
  return f_size(_file);
}

void File::close() {
  if(isDirectory()){
    f_closedir(_dir);
    delete _dir;
    _dir = NULL;
  }else{
     f_close(_file);
    delete _file; 
    _file = NULL;
  }
}

// allows you to recurse into a directory
File File::openNextFile(uint8_t mode) {
    FRESULT res;
    UINT i;
    static FILINFO fno;
    static char path[257];
    strcpy(path, _name);
    for (;;) {
        res = f_readdir(_dir, &fno);                   /* Read a directory item */
        if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
        if (fno.fattrib & AM_DIR) {                    /* It is a directory */
            i = strlen(path);
            sprintf((char*)path+i, "/%s",  fno.fname);
            DIR dir;
            if((res = f_opendir(&dir, path)) == FR_OK){
               return File(dir, path);
            }
             else
               return File();
        } else {                                       
            i = strlen(path);
            sprintf((char*)path+i, "/%s",  fno.fname);
            FIL file;
            if((res = f_open(&file, path, mode)) == FR_OK){
              return File(file, path);
            }
            else
              return File();
        }
    }
    return File();
}


void File::rewindDirectory(void){  
  if (isDirectory())
     f_rewinddir(_dir);
}

File::operator bool() {
  if (_file || _dir) 
    return  true;
  return false;
}


 File FS::open(const char *filepath, uint8_t mode)
 {
   FRESULT ret = FR_OK;
   FILINFO v_fileinfo;

   if(!strcmp(filepath, "/"))
   {
      DIR dir;
      if((ret = f_opendir(&dir, _T("/"))) == FR_OK)
          return File(dir, filepath);
         else
          return File();
          
   }
   
   if((ret = f_stat(filepath, &v_fileinfo)) == FR_OK){
      if (v_fileinfo.fattrib & AM_DIR)
      {
         DIR dir;
         if((ret = f_opendir(&dir, filepath)) == FR_OK)
          return File(dir, filepath);
         else
          return File();
      }else{
         FIL file;
         if((ret = f_open(&file, filepath, mode)) == FR_OK)
           return File(file, filepath);
         else
           return File();
      }
   }else{
      FIL file;
         if((ret = f_open(&file, filepath, mode)) == FR_OK)
           return File(file, filepath);
         else
           return File();
   }
    
 }

boolean FS:: exists(const char *filepath)
{
   FRESULT ret = FR_OK;
   FILINFO v_fileinfo;
   if((ret = f_stat(filepath, &v_fileinfo)) == FR_OK){
      return true;
   }else{
      return false;
   }
}


boolean FS::mkdir(const char *filepath) 
{
  FRESULT ret = FR_OK;
  ret = f_mkdir(filepath);
  if(ret == FR_OK)
     return true;
  else
    return false;
}

boolean FS::rename(const char* pathFrom, const char* pathTo)
{
  FRESULT ret = FR_OK;
  ret = f_rename(pathFrom, pathTo);
  if(ret == FR_OK)
     return true;
  else
    return false;
}

boolean FS::rmdir(const char *filepath) 
{
   char file[_MAX_LFN+2]; 
   FRESULT status;
   DIR dj;
   FILINFO fno;
   status = f_findfirst(&dj, &fno, filepath, _T("*"));
    while (status == FR_OK && fno.fname[0]) {
        sprintf((char*)file, "%s/%s", filepath, fno.fname);
        if (fno.fattrib & AM_DIR){
            rmdir(file);
        }
        else{
            remove(file);
        }
        status = f_findnext(&dj, &fno);
    }
   f_closedir(&dj);
  if(remove(filepath))
  {
     return true;
  }else{
    return false;
  }
}

boolean FS::remove(const char *filepath)
{
  FRESULT ret = FR_OK;
  ret = f_unlink(filepath);
  if(ret == FR_OK)
     return true;
  else
    return false;
}
};
