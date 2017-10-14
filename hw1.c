#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

typedef enum  { REGFILE=0,DIRECTORY,SYMLINK }  FILE_TYPE;

typedef struct{
	
	 char path[200];
	 char name[200];
	 char parent[200];
	 char linkPath[200];
	 FILE_TYPE type;
	 long long size;
	 long modTime;

} File;

int numberOfFiles(char* path){
	
		int count = 0;
	    DIR *dir = opendir(path);
	    struct dirent* dent;
		if(!dir){
			printf("cannot open directory\n");
			closedir(dir);
			return 0;
		}
			
		while((dent=readdir(dir))!=NULL)
			count++;
		
		closedir(dir);	
		return count;
}

char* readLink(char* path){
	
	struct stat sb;
    char *linkname;
    ssize_t r;

   if (lstat(path, &sb) == -1) {
        perror("lstat");
        exit(EXIT_FAILURE);
    }

   linkname = malloc(sb.st_size + 1);
    if (linkname == NULL) {
        fprintf(stderr, "insufficient memory\n");
        exit(EXIT_FAILURE);
    }

   r = readlink(path, linkname, sb.st_size + 1);

   if (r < 0) {
        perror("lstat");
        exit(EXIT_FAILURE);
    }

   if (r > sb.st_size) {
        fprintf(stderr, "symlink increased in size "
                        "between lstat() and readlink()\n");
        exit(EXIT_FAILURE);
    }

   linkname[sb.st_size] = '\0';

   //printf("'%s' points to '%s'\n", argv[1], linkname);
	return linkname;
	
	
}

File** ls(char* path, int *numOfFiles){
	
		*numOfFiles = numberOfFiles(path)-2;
		
		if(*numOfFiles <= 0)
			return NULL;
		
		File** files = (File**) malloc((*numOfFiles)*sizeof(File*));
		
		DIR *dir = opendir(path);
		struct dirent* dent;
		int index=0;
		while((dent=readdir(dir))!=NULL){
			 
			File* f = (File*) malloc(sizeof(File));
			if(dent->d_name[0] == '.')
				continue;
				
			strcpy(f->parent,path);
			strcpy(f->path,path);
			strcat(f->path,"/");
			strcat( f->path,dent->d_name);
            strcpy(f->name,dent->d_name);
            struct stat sb;
            lstat(f->path, &sb);
            
            switch (sb.st_mode & S_IFMT) {
			   case S_IFDIR:  f->type = DIRECTORY; break;
			   case S_IFLNK:  f->type = SYMLINK;   break;
			   case S_IFREG:  f->type = REGFILE;   break;
           }
  
			if(f->type == SYMLINK){
				strcpy(f->linkPath,readLink(f->path));
			}
		   
		   f->modTime = sb.st_mtim.tv_sec + sb.st_mtim.tv_nsec / (long)1000000000.;
		   f->size = sb.st_size;
		    files[index] = f;
		    index++;
			
		}
		
		
		return files;
	
	
}

void synch(char* d1,char* d2,int unionMode){
	
	int n1=0,n2=0;	
	File** files1 = ls(d1,&n1);
	File** files2 = ls(d2,&n2);
	
	if(n1<=0 || n2<=0)
		return;
	
	int i=0;
	for(;i<n1;i++){
		
			int j=0;
			int matched = 0;
			for(;j<n2;j++){
					
					if(!strcmp(files1[i]->name,files2[j]->name)){
	
						matched = 1;
						
						if(files1[i]->type == DIRECTORY)
							synch(files1[i]->path,files2[j]->path,unionMode);
							
						else if(unionMode){
					
							if( (files1[i]->modTime > files2[j]->modTime) ){
								
								if(files1[i]->type == REGFILE && files2[j]->type == SYMLINK){
									printf("rm %s\n",files2[j]->path);
									printf("cp -p %s %s\n",files1[i]->path,files2[j]->parent);
								}
								else if(files1[i]->type == SYMLINK && files2[j]->type == REGFILE){
									printf("ln -sf %s %s\n",files1[i]->linkPath,files2[j]->path );
								}
								else if(files1[i]->type == SYMLINK && files2[j]->type == SYMLINK){
									
									//??????????????????????????????????????????
									printf("ln -sf %s %s\n",files1[i]->linkPath,files2[j]->path);
								}
								else {
									printf("cp -p %s %s\n",files1[i]->path,files2[j]->parent);
								}	
							}
							
							else if(files1[i]->modTime < files2[j]->modTime){
								
								if(files1[i]->type == REGFILE && files2[j]->type == SYMLINK){
									
									printf("ln -sf %s %s\n",files2[j]->linkPath,files1[i]->path );
								}
								else if(files1[i]->type == SYMLINK && files2[j]->type == REGFILE){
									
									printf("rm %s\n",files1[i]->path);
									printf("cp -p %s %s\n",files2[j]->path,files1[i]->parent);
								}
								else if(files1[i]->type == SYMLINK && files2[j]->type == SYMLINK){
									
									//??????????????????????????????????????????
									printf("ln -sf %s %s\n",files2[j]->linkPath,files1[i]->path);
								}
								else {
									printf("cp -p %s %s\n",files2[j]->path,files1[i]->parent);
								}	
							}
		
							else if(files1[i]->modTime == files2[j]->modTime){
								
								if(files1[i]->type == REGFILE && files2[j]->type == SYMLINK){
									
									printf("rm %s\n",files2[j]->path);
									printf("cp -p %s %s\n",files1[i]->path,files2[j]->parent);
								}
								else if(files1[i]->type == SYMLINK && files2[j]->type == REGFILE){
									
									printf("rm %s\n",files1[i]->path);
									printf("cp -p %s %s\n",files2[j]->path,files1[i]->parent);
								}
								else if(files1[i]->type == SYMLINK && files2[j]->type == SYMLINK){
									
									//??????????????????????????????????????????
									//printf("cp -p %s %s\n",files2[j]->path,files1[i]->parent);
								}
								else {
									
									if(files1[i]->size > files2[j]->size )
										printf("cp -p %s %s\n",files1[i]->path,files2[j]->parent);
									else if(files1[i]->size < files2[j]->size )
										printf("cp -p %s %s\n",files2[j]->path,files1[i]->parent);
								}	
								
							}
						
						} else{
							
								if( (files1[i]->modTime < files2[j]->modTime) ){
								
									if(files1[i]->type == REGFILE && files2[j]->type == SYMLINK){
										printf("rm %s\n",files2[j]->path);
										printf("cp -p %s %s\n",files1[i]->path,files2[j]->parent);
									}
									else if(files1[i]->type == SYMLINK && files2[j]->type == REGFILE){
										printf("ln -sf %s %s\n",files1[i]->linkPath,files2[j]->path );
									}
									else if(files1[i]->type == SYMLINK && files2[j]->type == SYMLINK){
										
										//??????????????????????????????????????????
										printf("ln -sf %s %s\n",files1[i]->linkPath,files2[j]->path);
									}
									else {
										printf("cp -p %s %s\n",files1[i]->path,files2[j]->parent);
									}	
							}
							
							else if(files1[i]->modTime > files2[j]->modTime){
								
									if(files1[i]->type == REGFILE && files2[j]->type == SYMLINK){
										
										printf("ln -sf %s %s\n",files2[j]->linkPath,files1[i]->path );
									}
									else if(files1[i]->type == SYMLINK && files2[j]->type == REGFILE){
										
										printf("rm %s\n",files1[i]->path);
										printf("cp -p %s %s\n",files2[j]->path,files1[i]->parent);
									}
									else if(files1[i]->type == SYMLINK && files2[j]->type == SYMLINK){
										
										//??????????????????????????????????????????
										printf("ln -sf %s %s\n",files2[j]->linkPath,files1[i]->path);
									}
									else {
										printf("cp -p %s %s\n",files2[j]->path,files1[i]->parent);
									}	
							}
		
							else if(files1[i]->modTime == files2[j]->modTime){
								
								if(files1[i]->type == REGFILE && files2[j]->type == SYMLINK){
									
									printf("ln -sf %s %s\n",files2[j]->linkPath,files1[i]->path );
								}
								else if(files1[i]->type == SYMLINK && files2[j]->type == REGFILE){
									
									printf("ln -sf %s %s\n",files1[i]->linkPath,files2[j]->path );
								}
								else if(files1[i]->type == SYMLINK && files2[j]->type == SYMLINK){
									
									//??????????????????????????????????????????
								}
								else {
									
									if(files1[i]->size < files2[j]->size )
										printf("cp -p %s %s\n",files1[i]->path,files2[j]->parent);
									else if(files1[i]->size > files2[j]->size )
										printf("cp -p %s %s\n",files2[j]->path,files1[i]->parent);
								}	
								
							}
						}
					}
			}
			
			if(!matched){
					if(unionMode){
						
						if(files1[i]->type == REGFILE){
								printf("cp -p %s %s\n",files1[i]->path,d2);
						}
						else if (files1[i]->type == DIRECTORY){
								printf("cp -rp %s %s\n",files1[i]->path,d2);
						}
						else {
							
							//??????????????????????????????????????????
							printf("ln -sf %s %s%s%s\n",files1[i]->linkPath,d2,"/",files1[i]->name);
						}
					}else{
						
						if(files1[i]->type == REGFILE){
								printf("rm %s\n",files1[i]->path);
						}
						else if (files1[i]->type == DIRECTORY){
								printf("rm -rf %s\n",files1[i]->path);
						}
						else {
							printf("rm %s\n",files1[i]->path);
							//??????????????????????????????????????????
						}
						
						
					}
			}	
	}
	
	i=0;
	for(;i<n2;i++){
		
			int j=0;
			int matched = 0;
			for(;j<n1;j++)
					if(!strcmp(files1[j]->name,files2[i]->name))
						matched = 1;
						
			if(!matched){
					if(unionMode){
						
						if(files2[i]->type == REGFILE){
								printf("cp -p %s %s\n",files2[i]->path,d1);
						}
						else if (files2[i]->type == DIRECTORY){
								printf("cp -rp %s %s\n",files2[i]->path,d1);
						}
						else {
							printf("ln -sf %s %s%s%s\n",files2[i]->linkPath,d1,"/",files2[i]->name);
							//??????????????????????????????????????????
						}
					}else{
						
						if(files2[i]->type == REGFILE){
								printf("rm %s\n",files2[i]->path);
						}
						else if (files2[i]->type == DIRECTORY){
								printf("rm -rf %s\n",files2[i]->path);
						}
						else {
							printf("rm %s\n",files2[i]->path);
							//??????????????????????????????????????????
						}
							
					}
			}
	}
}

int main(int argc,char** argv){
	
	
	if(argc < 3 || argc >4){
			printf("Usage:   synchronize [-i] dir1 dir2\n");
			return -1;
	}
	
	char* dir1 = argv[argc-2];
	char* dir2 = argv[argc-1];
	 
	 if(argc == 3)
			synch(dir1,dir2,1);
	 else
		if(!strcmp("-i",argv[1]))
			synch(dir1,dir2,0);
		else
			printf("Usage:   synchronize [-i] dir1 dir2\n");

	return 0;
}
