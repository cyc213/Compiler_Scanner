#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


void scan(FILE* fp);
bool whitespace(char c);
bool scan_iden(FILE *fp);
bool scan_rewd(FILE *fp);
bool scan_oper(FILE* fp);
bool scan_spec(FILE* fp);
bool scan_sc(FILE* fp);
bool scan_prep(FILE* fp);
bool scan_mc(FILE* fp);
int line_num = 0;

void scan(FILE* fp){

    if(scan_rewd(fp)){
        return;
    }else if(scan_spec(fp)){
        return;
    }else if(scan_prep(fp)){
        return;
    }else if(scan_sc(fp)){
        return;
    }else if(scan_oper(fp)){
        return;
    }else if(scan_iden(fp)){
        return;
    }else{
        char c = fgetc(fp);
        return;
    }

    // if(!scan_iden(fp)){
    //     fgetc(fp);
    // }
    // if(!scan_mc(fp)){
    //     fgetc(fp);
    //     return;
    // }
    
    return;
}

bool whitespace(char c){
    if(c == ' ' || c == '\t'){
        return true;
    }
    return false;
}


//識別字
bool scan_iden(FILE *fp){
    char c = fgetc(fp);
    
    if(c == '_' || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')){
        char temp[32] = {0};
        size_t temp_len = 0;
        while(c == '_' || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9')){
            temp[temp_len++] = c;
            c = fgetc(fp);
            
        }
        ungetc(c, fp);
        printf("IDEN\t%s\n", temp);
        return true;

    }else{
        ungetc(c, fp);
        return false;
    }
}

//保留字
bool scan_rewd(FILE *fp){
    char* rewds[31] = {
        "if", "else", "while", "for", "do", "switch", "case", "default", "continue", "int", 
        "long", "float", "double", "char", "break", "static", "extern", "auto", "register", 
        "sizeof", "union", "struct", "short", "enum", "return", "goto", "const", "signed", 
        "unsigned", "typedef", "void"};

    for(int i = 0; i < 31; i++){
        char* rewds_temp = rewds[i];
        size_t rewds_len = strlen(rewds_temp);
        char temp[rewds_len + 1];

        for(int j = 0; j < sizeof(temp); j++){
            temp[j] = 0;
        }

        fgets(temp, sizeof(temp), fp);
        

        if(!strcmp(temp, rewds_temp)){
            printf("REWD\t%s\n", temp);
            return true;
        }else{
            //printf("%s\n", temp);
            for(int i = strlen(temp) - 1; i >= 0; i--){
                ungetc(temp[i], fp);
            }
        }
    }
    return false;
}

//運算子
bool scan_oper(FILE* fp){
    //printf("into scan oper");
    char* opers[32] = {
        "+", "-", "*", "/", ",", "%", ">>", "<<", "++", "--", 
        "+=", "-=", "*=", "/=", "%=", "!", "&&", "||", "&", 
        "[", "]", "|", "^", ".", "->", ">", "<", "==", 
        ">=", "<=", "!=", ":"};
    
    for(int i = 0; i < 32; i++){
        char* opers_temp = opers[i];
        size_t opers_len = strlen(opers_temp);
        char temp[opers_len + 1];

        for(int j = 0; j < sizeof(temp); j++){
            temp[j] = 0;
        }

        fgets(temp, sizeof(temp), fp);
        
        //printf("%s\n", temp);
        if(!strcmp(temp, opers_temp)){
            printf("OPER\t%s\n", temp);
            return true;
        }else{
            for(int k = strlen(temp) - 1; k >= 0; k--){
                ungetc(temp[k], fp);
            }
            //printf("%s -> %s\n", temp, opers_temp);
        }
    }
    return false;
}

//特殊符號
bool scan_spec(FILE* fp){
    //printf("scan_spec...");
    char c = fgetc(fp);
    
    //printf("%c", c);
    if(c == '{' || c == '}' || c == '(' || c == ')' || c ==';'){
        printf("SPEC\t%c\n", c);
        return true;
    }else{
        ungetc(c, fp);
        return false;
    }
}

//單行註解
bool scan_sc(FILE* fp){
    char* sc = "//";
    char temp[strlen(sc) + 1];
    for(int i = 0; i < sizeof(temp); i++){
        temp[i] = 0;
    }
    fgets(temp, sizeof(temp), fp);
    
    //printf("%s", temp);
    
    if(!strcmp(sc, temp)){
        for(int i = strlen(temp) - 1; i >= 0; i--){
            ungetc(temp[i], fp);
        }
        
        char sc_text[256] = {0};
        
        size_t sctext_len = 0;

        char c = 0;
        do{
            c = fgetc(fp);
            
            sc_text[sctext_len++] = c;
            //printf("%s\n", sc_text);
        }while(c != 10 && c != EOF);
        sc_text[sctext_len - 1] = 0;

        printf("SC\t%s\n", sc_text);
        return true;
    }else{
        for(int i = strlen(temp) - 1; i >= 0; i--){
            ungetc(temp[i], fp);
        }
        return false;
    }
}

//前端處理
bool scan_prep(FILE* fp){
    char temp[128] = {0};
    size_t temp_len = 0;

    char c = fgetc(fp);
    
    if(c == '#'){
        temp[temp_len++] = c;
        
        do{
            c = fgetc(fp);
            
            temp[temp_len++] = c;
        }while(whitespace(c));
        ungetc(c, fp);
        temp[temp_len--] = 0;

        fgets(temp + temp_len, strlen("include") + 1, fp);

        if(!strcmp(temp + temp_len, "include")){
            strcpy(temp + temp_len, "include");
            temp_len += strlen("include");
        }else{
            printf("PREP\t%s\tERROR: expected \"include\"\n", temp);
            char* includeStr = "include";
            for(int k = strlen(includeStr) - 1; k >= 0; k--){
                ungetc(includeStr[k], fp);
            }
            return false;
        }

        c = fgetc(fp);
        
        while(whitespace(c)){
            temp[temp_len++] = c;
            c = fgetc(fp);
            
        }

        temp[temp_len++] = c;
        char close = 0;
        if(c == '<'){
            close = '>';
        }else if(c == '"'){
            close = '"';
        }else{
            ungetc(c, fp);
            printf("PREP\t%s\tERROR: expected < or \"\n", temp);
            return true;
        }

        do{
            c = getc(fp);
            temp[temp_len++] = c;
        }while(c != close && (c != 10 || c != 13));

        if(c == 10 || c == 13){
            temp[temp_len--] = 0;
        }

        if(c == close){
            printf("PREP\t%s\n", temp);
        }else{
            printf("PREP\t%s\tERROR: missing %c\n", temp, close);
        }
        return true;
    }else{
        ungetc(c, fp);
        return false;
    }
}

//多行註解
bool scan_mc(FILE* fp){
    char temp[strlen("/*") + 1];
    for(int i = 0; i < strlen(temp); i++){
        temp[i] = 0;
    }

    fgets(temp, strlen(temp), fp);
    if(!strcmp("/*", temp)){
        char c = 0;
        do{
            c = getc(fp);
            if(c == '*'){
                c = getc(fp);
                if(c == '/'){
                    printf("MC\t\n");
                    return true;
                }
            }
        }while(c != EOF);

    }else{
        for(int i = strlen(temp) - 1; i >= 0; i--){
            ungetc(temp[i], fp);
        }
        return false;
    }
    return false;
}

int main(){
    //printf("test..");

    FILE *fp = fopen("scanner.c", "r");
    char c = 0;
    line_num = 1;
    
    do{
        scan(fp);
        
        c = getc(fp);
        ungetc(c, fp);

        if(whitespace(c)){
            fgetc(fp);
            
        }
    }while(c != EOF);

    return 0;
}

