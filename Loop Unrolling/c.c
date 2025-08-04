#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM 1

long extractCount(char *ptr){
    int x = 0;
    while((ptr[x] != '<')&&(ptr[x] != '>')){
        x++;
    }
    long count = 0;
    sscanf(&ptr[x+1], "%ld;", &count);
    return count;
}

//MUST DECLARE VARIABLES WITHIN FOR LOOP
char extractVar(char *ptr){
    char var;
    sscanf(ptr, "int %c =", &var);
    return var;
}

long makeOptimization(long opt, long fullUnroll){ //where fullUnroll = count; ex: i < 125;
    int count = 0;
    switch (opt)
    {
    case 0:
        count = 2;
        break;
    case 1:
        count = 4;
        break;
    case 2: //fullUnroll used exclusively for fully unrolling the loop
        count = fullUnroll;
        break;
    default:
        printf("Error in makeOptimization Switch \n");
        break;
    }
    return count;
}

void editForLoop(char *variable, char *condition, char *increment, char *output, long optimization){

    char condArray[(strlen(condition)+5)];
    char incArray[(strlen(increment)+4)];
    //incArray[0] = '\0';
    strcpy(condArray, condition);
    strncpy(incArray, increment, 3);
    incArray[3] = '\0';

    //Based on Optimization
    switch (optimization)
    {
    case 0:
        strcat(condArray, " - 1");
        strcat(incArray, "=2");
        break;
    case 1:
        strcat(condArray, " - 3");
        strcat(incArray, "=4");
        break;
    case 2:

        break;
    default:
        break;
    }    

    //Building the newForLoop output
    strcpy(output, "for(");
    strcat(output, variable); 
    strcat(output, "; ");
    strcat(output, condArray);
    strcat(output, "; ");
    strcat(output, incArray);
    strcat(output, "){"); 
}

void editLine(char *linePtr, char varName, FILE *mod, int i) {
    char varString[2];
    varString[0] = varName;
    varString[1] = '\0';

    int length = strlen(linePtr);
    char *newLine = malloc(length + 51 * 10); //500 replacements MAX or else you a silly dog
    if (newLine == NULL) {
        printf("Memory allocation for newLine failed.\n");
        exit(0);
    }
    newLine[0] = '\0'; //empty string

    char numStr[10];
    sprintf(numStr, "%d", i); // Convert i - which is a number - to a string

    int num = 0;
    while (num < length) {
        // Look for the next occurrence of varName
        if (linePtr[num] == varName && (linePtr[num + 1] == ']' || linePtr[num + 1] == ' ' || linePtr[num + 1] == ';')) {
            // Copy everthing up until now
            strncat(newLine, linePtr, num);
            // Add in anything else
            strcat(newLine, "(");
            strcat(newLine, varString);
            strcat(newLine, " + ");
            strcat(newLine, numStr);
            strcat(newLine, ")");
            // Need to move the pointer to the next 
            linePtr += num + 1; // Put the pointer at num characters + 1
            length -= num + 1; //Remaining length gets rid of what you read and one character BECAUSE you have to start at the beginning again, but now the length of the string is changed. 
            num = 0; // Restart from the beginning of the remaining string
        } else {
            num++;
        }
    }

    // Any remaining 
    strcat(newLine, linePtr);

    // Write to file
    fprintf(mod, "%s", newLine);

    free(newLine); // Free allocated memory
}


void writeNewFile(){
    //will figure out if necessary, don't want to waste time on this. char file[] = argv[2]; not allowed
    FILE *fptr;
    fptr = fopen("test_modified.c","w");
}

void constrcutFinalForLoop(){

}

int main(int argc, char *argv[])
{
    if(argc < 3) {
        printf("Usage: %s x yyy.c where x is a number between 0 and 2 and yyy.c is a C source file.\n", argv[0]);
        return 0;
    }
    //char file[] = argv[2];
    printf("%s\n", argv[2]);
    long optLevel = strtol(argv[1], NULL, 10);

    FILE *fp = NULL;

    if((fp = fopen(argv[2], "r")) != NULL)
    {
        char *line = NULL;
        int lineNum = 0;
        size_t len = 0;
        ssize_t charsRead;

        //Open A new File
        FILE *modified;
        modified = fopen("test_modified.c","w");

        while(!feof(fp))
        {
            if((charsRead = getline(&line, &len, fp)) != -1) {
                // printf("Retrieved line of length %zu:\n", charsRead);
                // printf("%d) %s\n", ++lineNum, line);


                ++lineNum;
                char *forPtr = NULL;

                //Finding a For Loop
                if((forPtr = strstr(line, "for")) != NULL) 
                {
                    if(forPtr[3] == '(' || (forPtr[3] == ' ' && forPtr[4] == '('))
                    {
                        printf("\nFound for loop on line %d here: %s", lineNum, forPtr);
                
                        //Extract endIteration as count
                        long count = extractCount(forPtr); //for full unrolling
                        long opt = makeOptimization(optLevel, count);

                        //Make a copy of "for" line   
                        char *forLine = malloc(strlen(forPtr) + 1); //REMEMBER TO FREE
                        //forLine = forPtr; Doesn't work because you need to allocate using strcpy(dest, src);
                        if(forLine == NULL){
                            printf("Memory forLine allocation failed. \n");
                            exit(0);
                        }
                        strcpy(forLine, forPtr);

                        //Edit "for" line by breaking it into tokens
                        char *forToken = strtok(forLine, "("); //unneeded but helps me get to var
                        char *varToken = strtok(NULL, ";"); 
                        char *condToken = strtok(NULL, ";");
                        char *incToken = strtok(NULL, ")");

                        //Extract variable name; Need for editing each individual line
                        char variable = extractVar(varToken); 
                        char *varPtr = &variable;

                        //Write A new ForLoop (if optLevel != 2)
                        if(optLevel < 2){
                        char *newForLoop = malloc(50); //not scalable, remember to free
                        editForLoop(varToken, condToken, incToken, newForLoop, optLevel);
                        // printf("newForLoop: %s\n", newForLoop);
                        
                        //Print the new for loop declaration to c file
                        fprintf(modified, newForLoop);
                        free(newForLoop);
                        }
                        //Write the last for Loop to a new file
                        FILE *temp; 
                        temp = fopen("temp.txt", "w+");
                        char *lastForLoop = malloc(100); //again, not scalable, remember to free
                        char varAsString[2]; //turn Variable into a string
                        varAsString[0] = variable;
                        varAsString[1] = '\0';
                        char numAsString[10]; //Make new starting value
                        sprintf(numAsString, "%d", (count - (count % opt)));
                        strcpy(lastForLoop, "for(");
                        strcat(lastForLoop, "int ");
                        strcat(lastForLoop, varAsString);
                        strcat(lastForLoop, " = ");
                        strcat(lastForLoop, numAsString);
                        strcat(lastForLoop, ";");
                        strcat(lastForLoop, condToken);
                        strcat(lastForLoop, ";");
                        strcat(lastForLoop, incToken);
                        strcat(lastForLoop, "){");
                        fprintf(temp, lastForLoop);
                        


                        //Change the Body of the For loop
                            getline(&line, &len, fp);
                            fprintf(modified, "\n");
                            fprintf(modified, line);
                            fprintf(temp, "\n"); //last for loop 
                            fprintf(temp, line); //last for loop - original line
                            char *newLine = malloc(strlen(line) + 51);
                            if(newLine == NULL){
                                printf("Memory newLine allocation failed. \n");
                                exit(0);
                            }
                            strcpy(newLine, line);

                            //Unroll First Line
                            for(int i = 1; i < opt; i++){
                                editLine(newLine, variable, modified, i);//will print out all instances of line unrolled to optlevel
                            }
                            free(newLine);

                            // //Unroll All other Lines - FAILED
                            // while(strchr(line, '}') == NULL )
                            // {
                            //     getline(&line, &len, fp);    EXECUTES - THEN YOU HAVE TO CHECK.
                            //     fprintf(modified, line); 
                            //     for(int i = 1; i < opt; i++){
                            //     editLine(line, variable, modified, i);   
                            //     }
                            //     lineNum++;
                            // }

                            //Unroll All the Other Times - WORKING
                            while (!0) {
                                getline(&line, &len, fp); // Read a line FIRST BECAUSE you have to CHECK!!!!
                                fprintf(temp, line); //Last for Loop
                                if (strchr(line, '}') != NULL) { // Check for '}'
                                break; // Exit the loop before processing further
                                }
                                fprintf(modified, line);
                                for (int i = 1; i < opt; i++) {
                                editLine(line, variable, modified, i);
                                }
                                lineNum++;
                            }
                            fclose(temp), temp = NULL;
                            temp = fopen("temp.txt", "r");
                            if(optLevel < 2)
                            {
                                fprintf(modified, "}\n");
                                char *line2 = NULL;
                                // int lineNum2 = 0;
                                size_t len2 = 0;
                                ssize_t charsRead2;
                                 char ch;
                                while(!feof(temp))
                                { 
                                    charsRead2 = getline(&line2, &len2, temp);
                                    printf("charsRead2: %d line2: %s\n", charsRead2,line2);
                                    if(charsRead2 != -1)
                                    {
                                    fprintf(modified, line2);
                                    } else {
                                        perror("getLine\n");
                                    }

                                }
                            }
                            
                        //TODO: WRITE IN extra for Loops if there are remaining times needed to be executed.
                        //free(forLine);  STILL NEED TO DO THIS??? 
                        

                //Add the final for loop in case it wasn't unrolled all the way

                    } else {
                        printf("Found non-loop for on line %d here: %s\n", lineNum, forPtr);
                    }


                } else { //Technically should be written the opposite way because the for loop will occur less times. But will fix later
                    fprintf(modified, line);
                }
                free(line), line = NULL;
            }
            if(line)
            {
                free(line), line = NULL;
            }
            //add more code here
        }
        fclose(fp), fp = NULL;
    } else {
        printf("Error opening file: %s\n", argv[2]);
    }

}