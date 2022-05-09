#undef MH_DECL_ENUM_ELEMENT
#undef MH_BEGIN_ENUM
#undef MH_END_ENUM

#ifndef MH_RUNTIME
#ifndef MH_GENERATE_ENUM_STRINGS
#define MH_DECL_ENUM_ELEMENT( element ) element
#define MH_BEGIN_ENUM( ENUM_NAME ) enum class ENUM_NAME
#define MH_END_ENUM( ENUM_NAME ) ; \
            char* GetString##ENUM_NAME(enum ENUM_NAME index);
#else
#define MH_DECL_ENUM_ELEMENT( element ) #element
#define MH_BEGIN_ENUM( ENUM_NAME ) char* gs_##ENUM_NAME [] =
#define MH_END_ENUM( ENUM_NAME ) ; char* GetString##ENUM_NAME(enum \
            ENUM_NAME index){ return gs_##ENUM_NAME [index]; }
#endif
#else
#define MH_DECL_ENUM_ELEMENT( element ) element
#define MH_BEGIN_ENUM( ENUM_NAME ) enum class ENUM_NAME
#define MH_END_ENUM( ENUM_NAME ) ;
#endif