#include "librnd.h"

struct RngArgs
{
    FILE      *rng;
    char      *buffer;
    long long  count;
};

my_bool urandom_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
    struct RngArgs *ra = NULL;
    FILE *rng = NULL;
    char *buffer = NULL;
    long long count = 0;
    
    if (args->arg_count == 0)
    {
        count = DEFAULT_ENTROPY;
    }
    else if (args->arg_count == 1)
    {
        if (args->arg_type[0] != INT_RESULT)
        {
            strncpy(message, "First argument must be an integer!", MYSQL_ERRMSG_SIZE);
            
            return 1;
        }
        else
        {
            count = *((long long*)(args->args[0]));
            
            if (count < MIN_BYTES)
            {
                strncpy(message, "The requested number of bytes is too low!", MYSQL_ERRMSG_SIZE);
                
                return 1;
            }
            else if (count > MAX_BYTES)
            {
                strncpy(message, "The requested number of bytes is too high!", MYSQL_ERRMSG_SIZE);
                
                return 1;
            }
        }
    }
    else
    {
        strncpy(message, "Invalid number of arguments!", MYSQL_ERRMSG_SIZE);
        
        return 1;
    }
    
    rng = fopen(RNG_DEV, "rb");
    
    if (rng == NULL)
    {
        strncpy(message, "Failed to open the external random number generator!", MYSQL_ERRMSG_SIZE);
        
        return 1;
    }
    
    buffer = calloc(count, sizeof(char));
    
    if (buffer == NULL)
    {
        if (rng != NULL)
        {
            fclose(rng);
            
            rng = NULL;
        }
        
        strncpy(message, "Failed to allocate memory!", MYSQL_ERRMSG_SIZE);
        
        return 1;
    }
    
    ra = malloc(sizeof(struct RngArgs));
    
    if (ra == NULL)
    {
        if (rng != NULL)
        {
            fclose(rng);
            
            rng = NULL;
        }
        
        if (buffer != NULL)
        {
            free(buffer);
            
            buffer = NULL;
        }
        
        strncpy(message, "Failed to allocate memory!", MYSQL_ERRMSG_SIZE);
        
        return 1;
    }
    
    ra->rng = rng;
    ra->buffer = buffer;
    ra->count = count;
    
    initid->ptr = (char*)(ra);
    
    return 0;
}

void urandom_deinit(UDF_INIT *initid)
{
    struct RngArgs *ra = NULL;
    
    ra = (struct RngArgs*)(initid->ptr);
    
    if (ra == NULL)
    {
        return;
    }
    
    if (ra->rng != NULL)
    {
        fclose(ra->rng);
        
        ra->rng = NULL;
    }
    
    if (ra->buffer != NULL)
    {
        free(ra->buffer);
        
        ra->buffer = NULL;
    }
    
    free(ra);
    
    ra = NULL;
    
    initid->ptr = NULL;
}

char *urandom(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error)
{
    struct RngArgs *ra = NULL;
    size_t read = 0;
    
    ra = (struct RngArgs*)(initid->ptr);
    
    read = fread(ra->buffer, sizeof(char), ra->count, ra->rng);
    
    if (read != ra->count)
    {
        *length = 0;
        *is_null = 1;
        *error = 1;
        
        return NULL;
    }
    
    *length = ra->count;
    
    return ra->buffer;
}
