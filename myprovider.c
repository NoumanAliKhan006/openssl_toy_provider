#include "myprovider.h"

#define COMMITID "123456789abcdef"
#define E_UNSUPPORTED           1

// Structure to hold provider context information
struct provctx_st {
    OSSL_LIB_CTX *libctx;
    const OSSL_CORE_HANDLE *handle;
    OSSL_FUNC_core_new_error_fn *up_new_error;
    OSSL_FUNC_core_set_error_debug_fn *up_set_error_debug;
    OSSL_FUNC_core_vset_error_fn *up_vset_error;
};

// Function to handle errors within the provider
static void err(struct provctx_st *pctx, int reason,
                const char *file, unsigned int line, const char *func,
                const char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  pctx->up_new_error(pctx->handle);
  pctx->up_set_error_debug(pctx->handle, file, line, func);
  pctx->up_vset_error(pctx->handle, reason, fmt, ap);
  va_end(ap);
}


// Macro for easier error handling
#define ERR(pctx,reason,fmt,...) \
    err((pctx), (reason), __FILE__, __LINE__, __func__, fmt     \
        __VA_OPT__(,) __VA_ARGS__)

  

// Function to return reason strings for error reporting
const OSSL_ITEM *get_reasons(void *provctx)
{
  (void)provctx;
  static const OSSL_ITEM reasons[] = {
    { E_UNSUPPORTED, "Hello, world!  This is unsupported" },
    { 0, NULL }
  };
  return reasons;
}




// Dummy implementation for hash operations
static const OSSL_DISPATCH blargh_hash_impl[] = {
    
    //{ OSSL_FUNC_DIGEST_GET_PARAMS, (void (*)(void))blargh_h_get_params },
    { 0, NULL }
    
    
};
// Algorithm definition for the 'blargh' algorithm
static const OSSL_ALGORITHM hashes[] = {
    { "BLARG:id-blargh:1.2.3.4.5", "x.author=bleah", blargh_hash_impl,
       "BLARG is a fictious generic algorithm" },
    { NULL, NULL, NULL, NULL }
};


// Function to query supported operations

static OSSL_FUNC_provider_query_operation_fn query;
static const OSSL_ALGORITHM *query(void *provctx, int operation_id,
                                   int *no_cache)
{

    struct provctx_st *pctx = provctx;
    fprintf(stderr, "Query called with operation_id=%d\n", operation_id);

    // Handle unsupported operations
    if (operation_id != OSSL_OP_DIGEST) {
        ERR(pctx, E_UNSUPPORTED, "operation_id=%d is unsupported", operation_id);
        return NULL;
    }

    // Return the list of supported algorithms
    *no_cache = 0;
    switch (operation_id) {
    case OSSL_OP_DIGEST:
        return hashes;
    }
    return NULL;
}



// Function to clean up provider context
static OSSL_FUNC_provider_teardown_fn teardown;
void teardown(void *provctx)
{
    struct provctx_st *pctx = provctx;

    // Free the library context and provider context
    OSSL_LIB_CTX_free(pctx->libctx);
    free(provctx);
}



// Function to get the list of parameters that the provider supports
static OSSL_FUNC_provider_gettable_params_fn gettable_params;
static OSSL_FUNC_provider_get_params_fn get_params;



static const OSSL_PARAM *gettable_params(void *provctx)
{
    (void)provctx;
    static const OSSL_PARAM params[] = {
        { "buildinfo", OSSL_PARAM_UTF8_PTR, NULL, 0, 0 },
        { NULL, 0, NULL, 0, 0 }
    };
    return params;
}

// Function to retrieve parameter values
static int get_params(void *provctx, OSSL_PARAM *params)
{
    (void)provctx;
    for (; params != NULL && params->key != NULL; params++) {
        if (strcasecmp(params->key, "buildinfo") == 0) {
            if (params->data_type == OSSL_PARAM_UTF8_PTR) {
                const char **r = params->data;
                *r = "commit " COMMITID;
                params->return_size = strlen(*r);
            } else {
                return 0;
            }
        }
    }
    return 1;
}



// Dispatch table for provider functions
const OSSL_DISPATCH provfns[] = {
    { OSSL_FUNC_PROVIDER_GET_REASON_STRINGS, (void (*)(void))get_reasons },
    { OSSL_FUNC_PROVIDER_TEARDOWN, (void (*)(void))teardown },
    { OSSL_FUNC_PROVIDER_GETTABLE_PARAMS, (void (*)(void))gettable_params },
    { OSSL_FUNC_PROVIDER_GET_PARAMS, (void (*)(void))get_params },
    { OSSL_FUNC_PROVIDER_QUERY_OPERATION, (void (*)(void))query },
    { 0, NULL }
};



// Provider initialization function
OSSL_provider_init_fn OSSL_provider_init;
int OSSL_provider_init(const OSSL_CORE_HANDLE *handle,
                       const OSSL_DISPATCH *in,
                       const OSSL_DISPATCH **out,
                       void **provctx)
{

    struct provctx_st *pctx = malloc(sizeof(struct provctx_st));

    if (pctx == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        return 0;  // Allocation failure
    }

    memset(pctx, 0, sizeof(*pctx));
    *provctx = pctx;
    *out = provfns;


    OSSL_FUNC_core_new_error_fn *c_new_error;
    OSSL_FUNC_core_set_error_debug_fn *c_set_error_debug ;
    OSSL_FUNC_core_vset_error_fn *c_vset_error;

    // Retrieve function pointers from the core library
    for (; in != NULL && in->function_id; in++) {
        switch (in->function_id) {
        case OSSL_FUNC_CORE_NEW_ERROR:
            c_new_error = OSSL_FUNC_core_new_error(in);
            break;
        case OSSL_FUNC_CORE_SET_ERROR_DEBUG:
            c_set_error_debug = OSSL_FUNC_core_set_error_debug(in);
            break;
        case OSSL_FUNC_CORE_VSET_ERROR:
            c_vset_error = OSSL_FUNC_core_vset_error(in);
            break;
        }
    }
    // Initialize context with core function pointers
    pctx->handle = handle;
    pctx->up_new_error = c_new_error;
    pctx->up_set_error_debug = c_set_error_debug;
    pctx->up_vset_error = c_vset_error;

    
    return 1;
}