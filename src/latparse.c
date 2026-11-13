/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "latparse.y"


/*
The MIT License (MIT)

Copyright (c) Latino - Lenguaje de Programacion

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

/* bison -y -oparse.c parse.y */
#define YYDEBUG 1
#define YYENABLE_NLS 1
#define YYLEX_PARAM &yylval, &yylloc

#include <stddef.h>

#include "latino.h"
#include "latast.h"
#include "latlex.h"

#ifndef LAT_ERROR_FMT
#define LAT_ERROR_FMT "%s:%d:%d: %s\n"
#endif

#ifdef __linux
#include <libintl.h>
#define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#endif

int yyerror(struct YYLTYPE *yylloc_param, void *scanner, struct ast **root, const char *s);
int yylex (YYSTYPE * yylval_param,YYLTYPE * yylloc_param ,yyscan_t yyscanner);


#line 122 "latparse.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "latparse.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_NUMERICO = 3,                   /* NUMERICO  */
  YYSYMBOL_CADENA = 4,                     /* CADENA  */
  YYSYMBOL_IDENTIFICADOR = 5,              /* IDENTIFICADOR  */
  YYSYMBOL_VAR_ARGS = 6,                   /* VAR_ARGS  */
  YYSYMBOL_SI = 7,                         /* SI  */
  YYSYMBOL_O_SI = 8,                       /* O_SI  */
  YYSYMBOL_FIN = 9,                        /* FIN  */
  YYSYMBOL_SINO = 10,                      /* SINO  */
  YYSYMBOL_MIENTRAS = 11,                  /* MIENTRAS  */
  YYSYMBOL_REPETIR = 12,                   /* REPETIR  */
  YYSYMBOL_ROMPER = 13,                    /* ROMPER  */
  YYSYMBOL_CONTINUAR = 14,                 /* CONTINUAR  */
  YYSYMBOL_HASTA = 15,                     /* HASTA  */
  YYSYMBOL_FUNCION = 16,                   /* FUNCION  */
  YYSYMBOL_DESDE = 17,                     /* DESDE  */
  YYSYMBOL_RETORNO = 18,                   /* RETORNO  */
  YYSYMBOL_ELEGIR = 19,                    /* ELEGIR  */
  YYSYMBOL_CASO = 20,                      /* CASO  */
  YYSYMBOL_DEFECTO = 21,                   /* DEFECTO  */
  YYSYMBOL_ATRIBUTO = 22,                  /* ATRIBUTO  */
  YYSYMBOL_VERDADERO = 23,                 /* VERDADERO  */
  YYSYMBOL_FALSO = 24,                     /* FALSO  */
  YYSYMBOL_NULO = 25,                      /* NULO  */
  YYSYMBOL_EXPONENTE = 26,                 /* EXPONENTE  */
  YYSYMBOL_PARA = 27,                      /* PARA  */
  YYSYMBOL_EN = 28,                        /* EN  */
  YYSYMBOL_RANGO = 29,                     /* RANGO  */
  YYSYMBOL_IR = 30,                        /* IR  */
  YYSYMBOL_MAYOR_QUE = 31,                 /* MAYOR_QUE  */
  YYSYMBOL_MENOR_QUE = 32,                 /* MENOR_QUE  */
  YYSYMBOL_MAYOR_IGUAL = 33,               /* MAYOR_IGUAL  */
  YYSYMBOL_MENOR_IGUAL = 34,               /* MENOR_IGUAL  */
  YYSYMBOL_IGUAL_LOGICO = 35,              /* IGUAL_LOGICO  */
  YYSYMBOL_DIFERENTE = 36,                 /* DIFERENTE  */
  YYSYMBOL_Y_LOGICO = 37,                  /* Y_LOGICO  */
  YYSYMBOL_O_LOGICO = 38,                  /* O_LOGICO  */
  YYSYMBOL_INCREMENTO = 39,                /* INCREMENTO  */
  YYSYMBOL_DECREMENTO = 40,                /* DECREMENTO  */
  YYSYMBOL_CONCATENAR = 41,                /* CONCATENAR  */
  YYSYMBOL_CONCATENAR_IGUAL = 42,          /* CONCATENAR_IGUAL  */
  YYSYMBOL_MAS_IGUAL = 43,                 /* MAS_IGUAL  */
  YYSYMBOL_MENOS_IGUAL = 44,               /* MENOS_IGUAL  */
  YYSYMBOL_POR_IGUAL = 45,                 /* POR_IGUAL  */
  YYSYMBOL_ENTRE_IGUAL = 46,               /* ENTRE_IGUAL  */
  YYSYMBOL_MODULO_IGUAL = 47,              /* MODULO_IGUAL  */
  YYSYMBOL_REGEX = 48,                     /* REGEX  */
  YYSYMBOL_GLOBAL = 49,                    /* GLOBAL  */
  YYSYMBOL_CLASE = 50,                     /* CLASE  */
  YYSYMBOL_CONSTRUCTOR = 51,               /* CONSTRUCTOR  */
  YYSYMBOL_HEREDA = 52,                    /* HEREDA  */
  YYSYMBOL_SUPER = 53,                     /* SUPER  */
  YYSYMBOL_MI = 54,                        /* MI  */
  YYSYMBOL_ESTATICO = 55,                  /* ESTATICO  */
  YYSYMBOL_NUEVA = 56,                     /* NUEVA  */
  YYSYMBOL_57_ = 57,                       /* '='  */
  YYSYMBOL_58_ = 58,                       /* '?'  */
  YYSYMBOL_59_ = 59,                       /* ':'  */
  YYSYMBOL_60_ = 60,                       /* '+'  */
  YYSYMBOL_61_ = 61,                       /* '-'  */
  YYSYMBOL_62_ = 62,                       /* '*'  */
  YYSYMBOL_63_ = 63,                       /* '/'  */
  YYSYMBOL_64_ = 64,                       /* '%'  */
  YYSYMBOL_65_ = 65,                       /* '!'  */
  YYSYMBOL_66_ = 66,                       /* '^'  */
  YYSYMBOL_67_ = 67,                       /* '('  */
  YYSYMBOL_68_ = 68,                       /* ')'  */
  YYSYMBOL_69_ = 69,                       /* '['  */
  YYSYMBOL_70_ = 70,                       /* ']'  */
  YYSYMBOL_71_ = 71,                       /* ';'  */
  YYSYMBOL_72_ = 72,                       /* ','  */
  YYSYMBOL_73_ = 73,                       /* '{'  */
  YYSYMBOL_74_ = 74,                       /* '}'  */
  YYSYMBOL_YYACCEPT = 75,                  /* $accept  */
  YYSYMBOL_constant_expression = 76,       /* constant_expression  */
  YYSYMBOL_primary_expression = 77,        /* primary_expression  */
  YYSYMBOL_unary_expression = 78,          /* unary_expression  */
  YYSYMBOL_multiplicative_expression = 79, /* multiplicative_expression  */
  YYSYMBOL_additive_expression = 80,       /* additive_expression  */
  YYSYMBOL_relational_expression = 81,     /* relational_expression  */
  YYSYMBOL_equality_expression = 82,       /* equality_expression  */
  YYSYMBOL_logical_not_expression = 83,    /* logical_not_expression  */
  YYSYMBOL_logical_and_expression = 84,    /* logical_and_expression  */
  YYSYMBOL_logical_or_expression = 85,     /* logical_or_expression  */
  YYSYMBOL_ternary_expression = 86,        /* ternary_expression  */
  YYSYMBOL_concat_expression = 87,         /* concat_expression  */
  YYSYMBOL_expression = 88,                /* expression  */
  YYSYMBOL_program = 89,                   /* program  */
  YYSYMBOL_statement_list = 90,            /* statement_list  */
  YYSYMBOL_statement = 91,                 /* statement  */
  YYSYMBOL_incdec_statement = 92,          /* incdec_statement  */
  YYSYMBOL_variable_access = 93,           /* variable_access  */
  YYSYMBOL_field_designator = 94,          /* field_designator  */
  YYSYMBOL_global_declaration = 95,        /* global_declaration  */
  YYSYMBOL_declaration = 96,               /* declaration  */
  YYSYMBOL_labeled_statements = 97,        /* labeled_statements  */
  YYSYMBOL_labeled_statement_case = 98,    /* labeled_statement_case  */
  YYSYMBOL_labeled_statement_case_case = 99, /* labeled_statement_case_case  */
  YYSYMBOL_labeled_statement_default = 100, /* labeled_statement_default  */
  YYSYMBOL_selection_statement = 101,      /* selection_statement  */
  YYSYMBOL_osi_statements = 102,           /* osi_statements  */
  YYSYMBOL_osi_statement = 103,            /* osi_statement  */
  YYSYMBOL_iteration_statement = 104,      /* iteration_statement  */
  YYSYMBOL_goto_etiqueta = 105,            /* goto_etiqueta  */
  YYSYMBOL_jump_statement = 106,           /* jump_statement  */
  YYSYMBOL_jump_loop = 107,                /* jump_loop  */
  YYSYMBOL_function_definition = 108,      /* function_definition  */
  YYSYMBOL_function_anonymous = 109,       /* function_anonymous  */
  YYSYMBOL_function_call = 110,            /* function_call  */
  YYSYMBOL_argument_expression_list = 111, /* argument_expression_list  */
  YYSYMBOL_parameter_list = 112,           /* parameter_list  */
  YYSYMBOL_list_new = 113,                 /* list_new  */
  YYSYMBOL_list_items = 114,               /* list_items  */
  YYSYMBOL_dict_new = 115,                 /* dict_new  */
  YYSYMBOL_dict_items = 116,               /* dict_items  */
  YYSYMBOL_dict_item = 117,                /* dict_item  */
  YYSYMBOL_clase_propiedad = 118,          /* clase_propiedad  */
  YYSYMBOL_clase_propiedades = 119,        /* clase_propiedades  */
  YYSYMBOL_clase_funcion = 120,            /* clase_funcion  */
  YYSYMBOL_clase_funciones = 121,          /* clase_funciones  */
  YYSYMBOL_clase_constructor = 122,        /* clase_constructor  */
  YYSYMBOL_clase_metodo_estatico = 123,    /* clase_metodo_estatico  */
  YYSYMBOL_clase_sentencia = 124,          /* clase_sentencia  */
  YYSYMBOL_clase_sentencias = 125,         /* clase_sentencias  */
  YYSYMBOL_clase_declaracion = 126         /* clase_declaracion  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE) \
             + YYSIZEOF (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  87
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1373

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  75
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  52
/* YYNRULES -- Number of rules.  */
#define YYNRULES  159
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  318

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   311


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    65,     2,     2,     2,    64,     2,     2,
      67,    68,    62,    60,    72,    61,     2,    63,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    59,    71,
       2,    57,     2,    58,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    69,     2,    70,    66,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    73,     2,    74,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   165,   165,   166,   170,   171,   172,   173,   177,   181,
     188,   192,   196,   200,   204,   211,   215,   222,   223,   224,
     225,   229,   230,   231,   235,   239,   243,   247,   251,   255,
     256,   257,   258,   259,   260,   261,   262,   263,   264,   265,
     266,   267,   268,   269,   270,   271,   272,   273,   274,   275,
     276,   277,   278,   282,   285,   289,   294,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     318,   319,   323,   324,   325,   326,   330,   331,   335,   338,
     344,   345,   347,   349,   351,   353,   355,   357,   358,   362,
     365,   368,   374,   380,   381,   387,   393,   395,   397,   399,
     405,   406,   410,   413,   416,   422,   424,   426,   429,   434,
     439,   447,   451,   452,   453,   457,   458,   462,   468,   474,
     478,   479,   480,   486,   487,   488,   489,   497,   509,   513,
     514,   515,   527,   531,   532,   533,   537,   538,   547,   551,
     555,   560,   568,   574,   579,   587,   590,   596,   599,   605,
     606,   607,   608,   609,   610,   611,   615,   620,   628,   631
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "NUMERICO", "CADENA",
  "IDENTIFICADOR", "VAR_ARGS", "SI", "O_SI", "FIN", "SINO", "MIENTRAS",
  "REPETIR", "ROMPER", "CONTINUAR", "HASTA", "FUNCION", "DESDE", "RETORNO",
  "ELEGIR", "CASO", "DEFECTO", "ATRIBUTO", "VERDADERO", "FALSO", "NULO",
  "EXPONENTE", "PARA", "EN", "RANGO", "IR", "MAYOR_QUE", "MENOR_QUE",
  "MAYOR_IGUAL", "MENOR_IGUAL", "IGUAL_LOGICO", "DIFERENTE", "Y_LOGICO",
  "O_LOGICO", "INCREMENTO", "DECREMENTO", "CONCATENAR", "CONCATENAR_IGUAL",
  "MAS_IGUAL", "MENOS_IGUAL", "POR_IGUAL", "ENTRE_IGUAL", "MODULO_IGUAL",
  "REGEX", "GLOBAL", "CLASE", "CONSTRUCTOR", "HEREDA", "SUPER", "MI",
  "ESTATICO", "NUEVA", "'='", "'?'", "':'", "'+'", "'-'", "'*'", "'/'",
  "'%'", "'!'", "'^'", "'('", "')'", "'['", "']'", "';'", "','", "'{'",
  "'}'", "$accept", "constant_expression", "primary_expression",
  "unary_expression", "multiplicative_expression", "additive_expression",
  "relational_expression", "equality_expression", "logical_not_expression",
  "logical_and_expression", "logical_or_expression", "ternary_expression",
  "concat_expression", "expression", "program", "statement_list",
  "statement", "incdec_statement", "variable_access", "field_designator",
  "global_declaration", "declaration", "labeled_statements",
  "labeled_statement_case", "labeled_statement_case_case",
  "labeled_statement_default", "selection_statement", "osi_statements",
  "osi_statement", "iteration_statement", "goto_etiqueta",
  "jump_statement", "jump_loop", "function_definition",
  "function_anonymous", "function_call", "argument_expression_list",
  "parameter_list", "list_new", "list_items", "dict_new", "dict_items",
  "dict_item", "clase_propiedad", "clase_propiedades", "clase_funcion",
  "clase_funciones", "clase_constructor", "clase_metodo_estatico",
  "clase_sentencia", "clase_sentencias", "clase_declaracion", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-206)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-158)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     730,   -40,  -206,   570,   570,   758,  -206,  -206,    20,   -23,
     570,   570,    29,    65,    42,    85,    31,  -206,   110,  -206,
     178,  -206,   384,  -206,  -206,  -206,  -206,  -206,  -206,  -206,
    -206,  -206,   487,   -49,  -206,  -206,  -206,  -206,    72,  -206,
      48,  -206,  -206,  -206,    50,   121,   570,   570,   570,   570,
     570,   593,  -206,  -206,  -206,  -206,  -206,  -206,  -206,  -206,
    -206,  -206,  -206,  -206,   665,  -206,   176,  -206,   217,  -206,
    -206,   665,   117,    69,    45,   886,  -206,   761,   115,  -206,
      68,   112,  -206,  -206,  -206,    11,    77,  -206,  -206,   143,
    -206,  -206,   570,   570,   570,   570,   570,   570,   570,   570,
     570,   570,    81,   113,   379,    84,   -14,   -14,   -14,  1093,
     928,    92,   179,  1171,   -15,  -206,   570,   570,   570,   570,
     570,   570,   570,   570,   570,   570,   570,   570,   570,   570,
     570,   570,   570,   570,    97,   570,   154,   570,   113,    94,
     570,   118,   109,   161,   104,  -206,   142,   -42,   167,   570,
     106,   170,   160,  -206,   172,  -206,   162,  -206,  -206,  -206,
      37,   173,  -206,   886,   120,  -206,  1249,  1249,  1249,  1249,
    1249,  1249,  1249,   125,  1011,  -206,  -206,  -206,  -206,   -35,
    -206,   131,   499,  -206,   570,  -206,   570,   570,   593,  -206,
    -206,   408,   408,   408,   408,   408,   408,  1307,  1307,  1288,
     408,  1210,   140,   140,   -14,   -14,   -14,   174,   570,  -206,
     758,   119,  -206,  1052,  -206,  1249,   -33,   570,  -206,   127,
     758,   758,  -206,  -206,   144,   570,   145,   886,  -206,     5,
      98,   182,   150,  -206,  -206,  -206,  -206,  -206,  -206,   152,
     758,  -206,  -206,   155,  -206,  1249,  1249,  -206,   570,   665,
     204,  -206,  -206,  -206,   758,   970,   202,  -206,  -206,   570,
    1249,   113,   758,   -32,   220,   159,   570,   224,  -206,  1249,
      87,  -206,   227,   758,   118,   800,   -27,   228,   758,  -206,
      16,  1249,  -206,   758,   233,  -206,   183,  -206,   758,   570,
     758,  -206,   238,   758,   -12,  -206,   758,   243,   843,   246,
    -206,   251,   758,   252,  -206,   758,   570,  -206,  -206,   254,
    -206,   255,  1132,  -206,  -206,   758,   256,  -206
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
     123,    72,   125,     0,     0,   123,   115,   116,     0,     0,
     120,     0,     0,     0,   123,     0,     0,    73,     0,    54,
      56,    64,     0,    74,    58,    57,    59,    60,    66,    61,
      65,    62,    63,     0,    67,   111,     2,     3,     4,    48,
       0,     5,     6,     7,     0,     0,     0,     0,     0,     0,
     129,   133,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    46,    40,   123,    47,    44,    45,    41,    42,
      43,   123,     0,     0,   123,   112,   113,     0,     0,   114,
      72,     0,    78,    79,    75,   123,     0,     1,    55,     0,
      70,    71,     0,     0,     0,     0,     0,     0,     0,   120,
       0,   120,     0,   123,     0,     0,     9,     8,    24,     0,
     130,     0,     4,     0,     0,   134,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   123,     0,
     120,     0,     0,     0,    90,    91,     0,    72,     0,   120,
       0,     0,     0,   153,   141,   151,   144,   152,   149,   150,
     123,     0,    69,   121,     0,    76,    81,    82,    83,    84,
      85,    86,    80,     0,     0,    87,   126,   127,   124,     0,
      50,     0,     0,    29,   129,   128,     0,     0,   136,   132,
      11,    17,    18,    19,    20,    22,    21,    25,    26,    28,
      23,     0,    16,    15,    12,    13,    14,    10,     0,    96,
     123,     0,   100,     0,   105,   106,     0,     0,   122,     0,
     123,   123,    99,    89,     0,     0,     0,   121,   155,   123,
     123,     0,     0,   140,   143,   156,   159,    68,   119,    77,
     123,    49,    52,     0,   131,   137,   138,   135,     0,   123,
       0,    98,   101,    77,   123,     0,    93,    92,    95,     0,
     139,   123,   123,     0,     0,     0,     0,     0,    51,    27,
     102,    97,     0,   123,     0,     0,     0,     0,   123,   158,
     123,    88,   118,   123,   104,   117,     0,    94,   123,     0,
     123,   146,     0,   123,     0,   103,   123,     0,     0,     0,
     145,     0,   123,     0,   108,   123,     0,   142,   148,     0,
     107,     0,     0,   147,   109,   123,     0,   110
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -206,  -139,  -206,  -206,  -206,  -206,  -206,  -206,  -206,  -206,
    -206,  -206,  -206,   363,  -206,   103,   -31,    49,     0,  -206,
    -206,   -13,   123,  -206,    -6,  -206,  -206,     1,  -205,  -206,
    -206,  -206,  -206,   258,  -206,     4,   -73,  -100,  -206,    90,
    -206,  -206,    88,  -206,   126,  -206,   129,  -206,  -206,  -206,
    -153,  -206
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,   163,    18,    19,    20,    65,    66,    23,
      24,    25,   143,   144,   220,   145,    26,   211,   212,    27,
      28,    29,    30,    31,    67,    68,    76,    33,    69,   111,
      70,   114,   115,   154,   155,   156,   157,   158,   159,   160,
     161,    34
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      22,    82,   219,   179,    32,    22,   252,   235,   101,    32,
     178,     2,   116,   164,    81,   225,   147,     2,    84,    35,
      22,   178,     2,   102,    32,    73,   173,   148,   175,   149,
    -124,   181,  -124,   240,    78,   254,   278,   102,   216,   102,
     102,   290,   147,     2,    74,   102,  -157,    80,     2,    21,
      80,     2,   133,   148,    21,   149,   302,   188,     8,   189,
     102,   139,   150,   151,    22,    17,   152,   218,    32,    21,
      79,    22,   153,   262,    81,    32,   228,   264,    84,   252,
      36,    37,    38,    39,   293,    81,   176,   177,   150,    84,
      85,    17,   152,    40,   -72,   208,    17,   283,    86,    17,
      41,    42,    43,   147,     2,   208,   209,   210,    72,   243,
      87,   -72,   -72,    21,   148,   103,   149,   104,   178,     2,
      21,    36,    37,    88,   141,   142,   105,   208,   251,   263,
      44,    17,   137,    45,    89,   219,   138,    46,    47,   -72,
    -124,   -72,    48,   146,    49,   162,    50,   153,   165,   150,
      51,   182,    17,   152,    92,    93,    94,    95,    96,    97,
      81,   276,   185,   214,    84,   217,   116,   134,   221,    98,
     222,   224,   226,   229,   136,   230,   231,   232,   148,    99,
     294,   100,   236,     1,     2,     3,   256,   265,   237,     4,
       5,     6,     7,   238,     8,     9,    10,    11,    89,   241,
     116,   -72,   130,   131,   132,    12,   133,   225,    13,   266,
      22,   259,   261,   271,    32,    90,    91,   153,   -72,   -72,
      22,    22,   274,   268,    32,    32,   280,    14,    15,   279,
      81,    16,    17,   282,    84,  -123,   285,   291,   186,   -75,
      22,   208,   286,    99,    32,   135,   -72,   300,   -72,    22,
    -123,   296,   304,    32,    22,   307,   -75,   -75,    32,    21,
     308,   310,    22,   313,   314,   317,    32,   223,   287,    21,
      21,   284,    83,    22,   244,     0,   247,    32,    22,     0,
     233,     0,    32,    22,   -75,   234,   -75,    32,    22,    21,
      22,     0,    32,    22,    32,     0,    22,    32,    21,     0,
      32,     0,    22,    21,     0,    22,    32,     0,     0,    32,
       0,    21,     0,   250,     0,    22,     0,     0,     0,    32,
       0,     0,    21,   257,   258,     0,     0,    21,     0,     0,
       0,     0,    21,     0,     0,     0,     0,    21,     0,    21,
       0,     0,    21,   267,     0,    21,     0,     0,     0,     0,
       0,    21,   270,     0,    21,     0,     0,   272,     0,     0,
       0,     0,     0,     0,    21,   277,    64,    71,     0,     0,
       0,     0,     0,    75,    77,     0,     0,     0,     0,     0,
       0,   292,    36,    37,    38,    39,   295,     0,     0,     0,
       0,   297,     0,   299,     0,    40,   301,     0,     0,   303,
       0,     0,    41,    42,    43,   309,    89,     0,   311,   106,
     107,   108,   109,   110,   113,     0,     0,     0,   316,     0,
       0,     0,     0,    90,    91,     0,    92,    93,    94,    95,
      96,    97,    44,    17,   116,    45,     0,     0,     0,    46,
      47,    98,     0,     0,    48,     0,    49,   180,    50,     0,
       0,    99,    51,   100,     0,   166,   167,   168,   169,   170,
     171,   172,     0,   174,     0,     0,     0,     0,   128,   129,
     130,   131,   132,     0,   133,     0,     0,     0,     0,   190,
     191,   192,   193,   194,   195,   196,   197,   198,   199,   200,
     201,   202,   203,   204,   205,   206,   207,     0,   213,     0,
     215,     0,    36,    37,    38,    39,     0,     0,     0,   -75,
       0,     0,   227,     0,     0,    40,     0,     0,     0,     0,
       0,     0,    41,    42,    43,     0,   -75,   -75,     0,   -75,
     -75,   -75,   -75,   -75,   -75,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   110,     0,   245,
     246,   113,    44,    17,   -75,    45,   -75,     0,     0,    46,
      47,     0,     0,     0,    48,     0,    49,   242,    50,     0,
       0,   249,    51,    36,    37,    38,    39,     0,     0,     0,
     255,     0,     0,     0,     0,     0,    40,     0,   260,     0,
       0,     0,     0,    41,    42,    43,    36,    37,   112,    39,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    40,
       0,   269,     0,     0,     0,     0,    41,    42,    43,     0,
       0,     0,   275,    44,    17,     0,    45,     0,     0,   281,
      46,    47,     0,     0,     0,    48,     0,    49,     0,    50,
       0,     0,     0,    51,     0,     0,    44,    17,     0,    45,
       0,     0,   298,    46,    47,     0,     0,     0,    48,     0,
      49,     0,    50,     0,     0,     0,    51,     0,     0,   312,
       1,     2,     3,     0,     0,     0,     4,     5,     6,     7,
       0,     8,     9,    10,    11,     0,     0,     0,     0,     0,
       0,   116,    12,     0,     0,    13,   117,   118,   119,   120,
     121,   122,   123,   124,     0,     0,   125,     0,     0,     0,
       0,     0,     0,   126,    14,    15,     0,     0,    16,    17,
       0,     0,     0,   127,     0,   128,   129,   130,   131,   132,
     -53,   133,     0,     0,     0,     1,     2,     3,     0,     0,
       0,     4,     5,     6,     7,     0,     8,     9,    10,    11,
       0,     0,     0,     0,     0,     0,     0,    12,     0,     0,
      13,     0,     0,     1,     2,     3,     0,     0,     0,     4,
       5,     6,     7,     0,     8,     9,    10,    11,     0,    14,
      15,   141,   142,    16,    17,    12,     0,   116,    13,     0,
       0,     0,   117,   118,   119,   120,   121,   122,   123,   124,
       0,     0,   125,     0,     0,     0,     0,    14,    15,   126,
       0,    16,    17,     0,     0,     0,     0,     0,     0,   127,
       0,   128,   129,   130,   131,   132,   116,   133,     0,     0,
       0,   117,   118,   119,   120,   121,   122,   123,   124,     0,
       0,   125,     0,     0,     0,     0,     0,     0,   126,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   127,     0,
     128,   129,   130,   131,   132,     0,   133,     0,   288,   116,
       0,     0,   289,     0,   117,   118,   119,   120,   121,   122,
     123,   124,     0,     0,   125,     0,     0,     0,     0,     0,
       0,   126,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   127,     0,   128,   129,   130,   131,   132,     0,   133,
       0,   305,   116,     0,     0,   306,     0,   117,   118,   119,
     120,   121,   122,   123,   124,     0,     0,   125,     0,     0,
       0,     0,     0,     0,   126,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   127,     0,   128,   129,   130,   131,
     132,     0,   133,     0,   116,     0,     0,     0,   140,   117,
     118,   119,   120,   121,   122,   123,   124,     0,     0,   125,
       0,     0,     0,     0,     0,     0,   126,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   127,     0,   128,   129,
     130,   131,   132,     0,   133,     0,   116,     0,     0,     0,
     184,   117,   118,   119,   120,   121,   122,   123,   124,     0,
       0,   125,     0,     0,     0,     0,     0,     0,   126,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   127,     0,
     128,   129,   130,   131,   132,     0,   133,   116,     0,     0,
       0,   273,   117,   118,   119,   120,   121,   122,   123,   124,
       0,     0,   125,     0,     0,     0,     0,     0,     0,   126,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   127,
       0,   128,   129,   130,   131,   132,     0,   133,   116,     0,
       0,   239,     0,   117,   118,   119,   120,   121,   122,   123,
     124,     0,     0,   125,     0,     0,     0,     0,     0,     0,
     126,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     127,     0,   128,   129,   130,   131,   132,     0,   133,   116,
       0,     0,   253,     0,   117,   118,   119,   120,   121,   122,
     123,   124,     0,     0,   125,     0,     0,     0,     0,     0,
       0,   126,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   127,     0,   128,   129,   130,   131,   132,   116,   133,
       0,   183,     0,   117,   118,   119,   120,   121,   122,   123,
     124,     0,     0,   125,     0,     0,     0,     0,     0,     0,
     126,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     127,     0,   128,   129,   130,   131,   132,   116,   133,     0,
     315,     0,   117,   118,   119,   120,   121,   122,   123,   124,
       0,     0,   125,     0,     0,     0,     0,     0,     0,   126,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   127,
     187,   128,   129,   130,   131,   132,   116,   133,     0,     0,
       0,   117,   118,   119,   120,   121,   122,   123,   124,     0,
       0,   125,     0,     0,     0,     0,     0,     0,   126,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   127,   248,
     128,   129,   130,   131,   132,   116,   133,     0,     0,     0,
     117,   118,   119,   120,   121,   122,   123,   124,     0,     0,
     125,     0,     0,     0,     0,     0,     0,   126,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   127,     0,   128,
     129,   130,   131,   132,   116,   133,     0,     0,     0,   117,
     118,   119,   120,   121,   122,   123,   124,     0,     0,   125,
       0,     0,     0,   116,     0,     0,   126,     0,   117,   118,
     119,   120,   121,   122,     0,     0,     0,     0,   128,   129,
     130,   131,   132,     0,   133,   126,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   128,   129,   130,
     131,   132,     0,   133
};

static const yytype_int16 yycheck[] =
{
       0,    14,   141,   103,     0,     5,   211,   160,    57,     5,
       5,     6,    26,    86,    14,    57,     5,     6,    14,    59,
      20,     5,     6,    72,    20,     5,    99,    16,   101,    18,
      72,   104,    72,    68,     5,    68,    68,    72,   138,    72,
      72,    68,     5,     6,    67,    72,     9,     5,     6,     0,
       5,     6,    66,    16,     5,    18,    68,    72,    16,    74,
      72,    74,    51,    52,    64,    54,    55,   140,    64,    20,
       5,    71,    85,    68,    74,    71,   149,   230,    74,   284,
       3,     4,     5,     6,    68,    85,     5,     6,    51,    85,
       5,    54,    55,    16,    22,     8,    54,    10,    67,    54,
      23,    24,    25,     5,     6,     8,     9,    10,     5,   182,
       0,    39,    40,    64,    16,    67,    18,    67,     5,     6,
      71,     3,     4,    20,    20,    21,     5,     8,     9,   229,
      53,    54,    15,    56,    22,   274,    67,    60,    61,    67,
      72,    69,    65,    28,    67,    68,    69,   160,     5,    51,
      73,    67,    54,    55,    42,    43,    44,    45,    46,    47,
     160,   261,    70,     9,   160,    71,    26,    64,    59,    57,
       9,    29,     5,    67,    71,     5,    16,     5,    16,    67,
     280,    69,     9,     5,     6,     7,    59,     5,    68,    11,
      12,    13,    14,    68,    16,    17,    18,    19,    22,    68,
      26,    22,    62,    63,    64,    27,    66,    57,    30,    57,
     210,    67,    67,     9,   210,    39,    40,   230,    39,    40,
     220,   221,    20,    68,   220,   221,    67,    49,    50,     9,
     230,    53,    54,     9,   230,    57,     9,     9,    59,    22,
     240,     8,   273,    67,   240,    69,    67,     9,    69,   249,
      72,    68,     9,   249,   254,     9,    39,    40,   254,   210,
       9,     9,   262,     9,     9,     9,   262,   144,   274,   220,
     221,   270,    14,   273,   184,    -1,   188,   273,   278,    -1,
     154,    -1,   278,   283,    67,   156,    69,   283,   288,   240,
     290,    -1,   288,   293,   290,    -1,   296,   293,   249,    -1,
     296,    -1,   302,   254,    -1,   305,   302,    -1,    -1,   305,
      -1,   262,    -1,   210,    -1,   315,    -1,    -1,    -1,   315,
      -1,    -1,   273,   220,   221,    -1,    -1,   278,    -1,    -1,
      -1,    -1,   283,    -1,    -1,    -1,    -1,   288,    -1,   290,
      -1,    -1,   293,   240,    -1,   296,    -1,    -1,    -1,    -1,
      -1,   302,   249,    -1,   305,    -1,    -1,   254,    -1,    -1,
      -1,    -1,    -1,    -1,   315,   262,     3,     4,    -1,    -1,
      -1,    -1,    -1,    10,    11,    -1,    -1,    -1,    -1,    -1,
      -1,   278,     3,     4,     5,     6,   283,    -1,    -1,    -1,
      -1,   288,    -1,   290,    -1,    16,   293,    -1,    -1,   296,
      -1,    -1,    23,    24,    25,   302,    22,    -1,   305,    46,
      47,    48,    49,    50,    51,    -1,    -1,    -1,   315,    -1,
      -1,    -1,    -1,    39,    40,    -1,    42,    43,    44,    45,
      46,    47,    53,    54,    26,    56,    -1,    -1,    -1,    60,
      61,    57,    -1,    -1,    65,    -1,    67,    68,    69,    -1,
      -1,    67,    73,    69,    -1,    92,    93,    94,    95,    96,
      97,    98,    -1,   100,    -1,    -1,    -1,    -1,    60,    61,
      62,    63,    64,    -1,    66,    -1,    -1,    -1,    -1,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,   130,   131,   132,   133,    -1,   135,    -1,
     137,    -1,     3,     4,     5,     6,    -1,    -1,    -1,    22,
      -1,    -1,   149,    -1,    -1,    16,    -1,    -1,    -1,    -1,
      -1,    -1,    23,    24,    25,    -1,    39,    40,    -1,    42,
      43,    44,    45,    46,    47,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   184,    -1,   186,
     187,   188,    53,    54,    67,    56,    69,    -1,    -1,    60,
      61,    -1,    -1,    -1,    65,    -1,    67,    68,    69,    -1,
      -1,   208,    73,     3,     4,     5,     6,    -1,    -1,    -1,
     217,    -1,    -1,    -1,    -1,    -1,    16,    -1,   225,    -1,
      -1,    -1,    -1,    23,    24,    25,     3,     4,     5,     6,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    16,
      -1,   248,    -1,    -1,    -1,    -1,    23,    24,    25,    -1,
      -1,    -1,   259,    53,    54,    -1,    56,    -1,    -1,   266,
      60,    61,    -1,    -1,    -1,    65,    -1,    67,    -1,    69,
      -1,    -1,    -1,    73,    -1,    -1,    53,    54,    -1,    56,
      -1,    -1,   289,    60,    61,    -1,    -1,    -1,    65,    -1,
      67,    -1,    69,    -1,    -1,    -1,    73,    -1,    -1,   306,
       5,     6,     7,    -1,    -1,    -1,    11,    12,    13,    14,
      -1,    16,    17,    18,    19,    -1,    -1,    -1,    -1,    -1,
      -1,    26,    27,    -1,    -1,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    -1,    -1,    41,    -1,    -1,    -1,
      -1,    -1,    -1,    48,    49,    50,    -1,    -1,    53,    54,
      -1,    -1,    -1,    58,    -1,    60,    61,    62,    63,    64,
       0,    66,    -1,    -1,    -1,     5,     6,     7,    -1,    -1,
      -1,    11,    12,    13,    14,    -1,    16,    17,    18,    19,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    27,    -1,    -1,
      30,    -1,    -1,     5,     6,     7,    -1,    -1,    -1,    11,
      12,    13,    14,    -1,    16,    17,    18,    19,    -1,    49,
      50,    20,    21,    53,    54,    27,    -1,    26,    30,    -1,
      -1,    -1,    31,    32,    33,    34,    35,    36,    37,    38,
      -1,    -1,    41,    -1,    -1,    -1,    -1,    49,    50,    48,
      -1,    53,    54,    -1,    -1,    -1,    -1,    -1,    -1,    58,
      -1,    60,    61,    62,    63,    64,    26,    66,    -1,    -1,
      -1,    31,    32,    33,    34,    35,    36,    37,    38,    -1,
      -1,    41,    -1,    -1,    -1,    -1,    -1,    -1,    48,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    58,    -1,
      60,    61,    62,    63,    64,    -1,    66,    -1,    68,    26,
      -1,    -1,    72,    -1,    31,    32,    33,    34,    35,    36,
      37,    38,    -1,    -1,    41,    -1,    -1,    -1,    -1,    -1,
      -1,    48,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    58,    -1,    60,    61,    62,    63,    64,    -1,    66,
      -1,    68,    26,    -1,    -1,    72,    -1,    31,    32,    33,
      34,    35,    36,    37,    38,    -1,    -1,    41,    -1,    -1,
      -1,    -1,    -1,    -1,    48,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    58,    -1,    60,    61,    62,    63,
      64,    -1,    66,    -1,    26,    -1,    -1,    -1,    72,    31,
      32,    33,    34,    35,    36,    37,    38,    -1,    -1,    41,
      -1,    -1,    -1,    -1,    -1,    -1,    48,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    58,    -1,    60,    61,
      62,    63,    64,    -1,    66,    -1,    26,    -1,    -1,    -1,
      72,    31,    32,    33,    34,    35,    36,    37,    38,    -1,
      -1,    41,    -1,    -1,    -1,    -1,    -1,    -1,    48,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    58,    -1,
      60,    61,    62,    63,    64,    -1,    66,    26,    -1,    -1,
      -1,    71,    31,    32,    33,    34,    35,    36,    37,    38,
      -1,    -1,    41,    -1,    -1,    -1,    -1,    -1,    -1,    48,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    58,
      -1,    60,    61,    62,    63,    64,    -1,    66,    26,    -1,
      -1,    70,    -1,    31,    32,    33,    34,    35,    36,    37,
      38,    -1,    -1,    41,    -1,    -1,    -1,    -1,    -1,    -1,
      48,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      58,    -1,    60,    61,    62,    63,    64,    -1,    66,    26,
      -1,    -1,    70,    -1,    31,    32,    33,    34,    35,    36,
      37,    38,    -1,    -1,    41,    -1,    -1,    -1,    -1,    -1,
      -1,    48,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    58,    -1,    60,    61,    62,    63,    64,    26,    66,
      -1,    68,    -1,    31,    32,    33,    34,    35,    36,    37,
      38,    -1,    -1,    41,    -1,    -1,    -1,    -1,    -1,    -1,
      48,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      58,    -1,    60,    61,    62,    63,    64,    26,    66,    -1,
      68,    -1,    31,    32,    33,    34,    35,    36,    37,    38,
      -1,    -1,    41,    -1,    -1,    -1,    -1,    -1,    -1,    48,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    58,
      59,    60,    61,    62,    63,    64,    26,    66,    -1,    -1,
      -1,    31,    32,    33,    34,    35,    36,    37,    38,    -1,
      -1,    41,    -1,    -1,    -1,    -1,    -1,    -1,    48,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    58,    59,
      60,    61,    62,    63,    64,    26,    66,    -1,    -1,    -1,
      31,    32,    33,    34,    35,    36,    37,    38,    -1,    -1,
      41,    -1,    -1,    -1,    -1,    -1,    -1,    48,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    58,    -1,    60,
      61,    62,    63,    64,    26,    66,    -1,    -1,    -1,    31,
      32,    33,    34,    35,    36,    37,    38,    -1,    -1,    41,
      -1,    -1,    -1,    26,    -1,    -1,    48,    -1,    31,    32,
      33,    34,    35,    36,    -1,    -1,    -1,    -1,    60,    61,
      62,    63,    64,    -1,    66,    48,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    60,    61,    62,
      63,    64,    -1,    66
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     5,     6,     7,    11,    12,    13,    14,    16,    17,
      18,    19,    27,    30,    49,    50,    53,    54,    89,    90,
      91,    92,    93,    94,    95,    96,   101,   104,   105,   106,
     107,   108,   110,   112,   126,    59,     3,     4,     5,     6,
      16,    23,    24,    25,    53,    56,    60,    61,    65,    67,
      69,    73,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    92,    93,   109,   110,   113,
     115,    88,    90,     5,    67,    88,   111,    88,     5,     5,
       5,    93,    96,   108,   110,     5,    67,     0,    90,    22,
      39,    40,    42,    43,    44,    45,    46,    47,    57,    67,
      69,    57,    72,    67,    67,     5,    88,    88,    88,    88,
      88,   114,     5,    88,   116,   117,    26,    31,    32,    33,
      34,    35,    36,    37,    38,    41,    48,    58,    60,    61,
      62,    63,    64,    66,    90,    69,    90,    15,    67,    96,
      72,    20,    21,    97,    98,   100,    28,     5,    16,    18,
      51,    52,    55,    96,   118,   119,   120,   121,   122,   123,
     124,   125,    68,    88,   111,     5,    88,    88,    88,    88,
      88,    88,    88,   111,    88,   111,     5,     6,     5,   112,
      68,   111,    67,    68,    72,    70,    59,    59,    72,    74,
      88,    88,    88,    88,    88,    88,    88,    88,    88,    88,
      88,    88,    88,    88,    88,    88,    88,    88,     8,     9,
      10,   102,   103,    88,     9,    88,   112,    71,   111,    76,
      99,    59,     9,    97,    29,    57,     5,    88,   111,    67,
       5,    16,     5,   119,   121,   125,     9,    68,    68,    70,
      68,    68,    68,   111,   114,    88,    88,   117,    59,    88,
      90,     9,   103,    70,    68,    88,    59,    90,    90,    67,
      88,    67,    68,   112,   125,     5,    57,    90,    68,    88,
      90,     9,    90,    71,    20,    88,   112,    90,    68,     9,
      67,    88,     9,    10,   102,     9,    91,    99,    68,    72,
      68,     9,    90,    68,   112,    90,    68,    90,    88,    90,
       9,    90,    68,    90,     9,    68,    72,     9,     9,    90,
       9,    90,    88,     9,     9,    68,    90,     9
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    75,    76,    76,    77,    77,    77,    77,    78,    78,
      79,    79,    79,    79,    79,    80,    80,    81,    81,    81,
      81,    82,    82,    82,    83,    84,    85,    86,    87,    88,
      88,    88,    88,    88,    88,    88,    88,    88,    88,    88,
      88,    88,    88,    88,    88,    88,    88,    88,    88,    88,
      88,    88,    88,    89,    89,    90,    90,    91,    91,    91,
      91,    91,    91,    91,    91,    91,    91,    91,    91,    91,
      92,    92,    93,    93,    93,    93,    94,    94,    95,    95,
      96,    96,    96,    96,    96,    96,    96,    96,    96,    97,
      97,    97,    98,    99,    99,   100,   101,   101,   101,   101,
     102,   102,   103,   103,   103,   104,   104,   104,   104,   104,
     104,   105,   106,   106,   106,   107,   107,   108,   109,   110,
     111,   111,   111,   112,   112,   112,   112,   112,   113,   114,
     114,   114,   115,   116,   116,   116,   117,   117,   117,   118,
     119,   119,   120,   121,   121,   122,   122,   123,   123,   124,
     124,   124,   124,   124,   124,   124,   125,   125,   126,   126
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     1,     1,     1,     1,     1,     2,     2,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     2,     3,     3,     5,     3,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     4,
       3,     5,     4,     0,     1,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     4,     3,
       2,     2,     1,     1,     1,     1,     3,     4,     2,     2,
       3,     3,     3,     3,     3,     3,     3,     3,     6,     2,
       1,     1,     3,     2,     4,     3,     4,     6,     5,     4,
       1,     2,     3,     5,     4,     4,     4,    10,     9,    11,
      13,     2,     2,     2,     2,     1,     1,     7,     6,     4,
       0,     1,     3,     0,     1,     1,     3,     3,     3,     0,
       1,     3,     3,     0,     1,     3,     0,     3,     3,     3,
       2,     1,     7,     2,     1,     6,     5,     8,     7,     1,
       1,     1,     1,     1,     2,     2,     2,     1,     6,     4
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (&yylloc, root, scanner, YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YYLOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

# ifndef YYLOCATION_PRINT

#  if defined YY_LOCATION_PRINT

   /* Temporary convenience wrapper in case some people defined the
      undocumented and private YY_LOCATION_PRINT macros.  */
#   define YYLOCATION_PRINT(File, Loc)  YY_LOCATION_PRINT(File, *(Loc))

#  elif defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static int
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  int res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
}

#   define YYLOCATION_PRINT  yy_location_print_

    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT(File, Loc)  YYLOCATION_PRINT(File, &(Loc))

#  else

#   define YYLOCATION_PRINT(File, Loc) ((void) 0)
    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT  YYLOCATION_PRINT

#  endif
# endif /* !defined YYLOCATION_PRINT */


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, Location, root, scanner); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, ast **root, void *scanner)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  YY_USE (yylocationp);
  YY_USE (root);
  YY_USE (scanner);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, ast **root, void *scanner)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  YYLOCATION_PRINT (yyo, yylocationp);
  YYFPRINTF (yyo, ": ");
  yy_symbol_value_print (yyo, yykind, yyvaluep, yylocationp, root, scanner);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp,
                 int yyrule, ast **root, void *scanner)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)],
                       &(yylsp[(yyi + 1) - (yynrhs)]), root, scanner);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule, root, scanner); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, ast **root, void *scanner)
{
  YY_USE (yyvaluep);
  YY_USE (yylocationp);
  YY_USE (root);
  YY_USE (scanner);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  switch (yykind)
    {
    case YYSYMBOL_NUMERICO: /* NUMERICO  */
#line 130 "latparse.y"
            { latA_destruir(((*yyvaluep).node)); }
#line 1509 "latparse.c"
        break;

    case YYSYMBOL_CADENA: /* CADENA  */
#line 130 "latparse.y"
            { latA_destruir(((*yyvaluep).node)); }
#line 1515 "latparse.c"
        break;

    case YYSYMBOL_IDENTIFICADOR: /* IDENTIFICADOR  */
#line 130 "latparse.y"
            { latA_destruir(((*yyvaluep).node)); }
#line 1521 "latparse.c"
        break;

    case YYSYMBOL_VAR_ARGS: /* VAR_ARGS  */
#line 130 "latparse.y"
            { latA_destruir(((*yyvaluep).node)); }
#line 1527 "latparse.c"
        break;

    case YYSYMBOL_constant_expression: /* constant_expression  */
#line 130 "latparse.y"
            { latA_destruir(((*yyvaluep).node)); }
#line 1533 "latparse.c"
        break;

    case YYSYMBOL_primary_expression: /* primary_expression  */
#line 130 "latparse.y"
            { latA_destruir(((*yyvaluep).node)); }
#line 1539 "latparse.c"
        break;

    case YYSYMBOL_unary_expression: /* unary_expression  */
#line 130 "latparse.y"
            { latA_destruir(((*yyvaluep).node)); }
#line 1545 "latparse.c"
        break;

    case YYSYMBOL_multiplicative_expression: /* multiplicative_expression  */
#line 130 "latparse.y"
            { latA_destruir(((*yyvaluep).node)); }
#line 1551 "latparse.c"
        break;

    case YYSYMBOL_additive_expression: /* additive_expression  */
#line 130 "latparse.y"
            { latA_destruir(((*yyvaluep).node)); }
#line 1557 "latparse.c"
        break;

    case YYSYMBOL_relational_expression: /* relational_expression  */
#line 130 "latparse.y"
            { latA_destruir(((*yyvaluep).node)); }
#line 1563 "latparse.c"
        break;

    case YYSYMBOL_equality_expression: /* equality_expression  */
#line 130 "latparse.y"
            { latA_destruir(((*yyvaluep).node)); }
#line 1569 "latparse.c"
        break;

    case YYSYMBOL_logical_not_expression: /* logical_not_expression  */
#line 130 "latparse.y"
            { latA_destruir(((*yyvaluep).node)); }
#line 1575 "latparse.c"
        break;

    case YYSYMBOL_logical_and_expression: /* logical_and_expression  */
#line 130 "latparse.y"
            { latA_destruir(((*yyvaluep).node)); }
#line 1581 "latparse.c"
        break;

    case YYSYMBOL_logical_or_expression: /* logical_or_expression  */
#line 130 "latparse.y"
            { latA_destruir(((*yyvaluep).node)); }
#line 1587 "latparse.c"
        break;

    case YYSYMBOL_ternary_expression: /* ternary_expression  */
#line 130 "latparse.y"
            { latA_destruir(((*yyvaluep).node)); }
#line 1593 "latparse.c"
        break;

    case YYSYMBOL_concat_expression: /* concat_expression  */
#line 130 "latparse.y"
            { latA_destruir(((*yyvaluep).node)); }
#line 1599 "latparse.c"
        break;

    case YYSYMBOL_expression: /* expression  */
#line 130 "latparse.y"
            { latA_destruir(((*yyvaluep).node)); }
#line 1605 "latparse.c"
        break;

    case YYSYMBOL_program: /* program  */
#line 130 "latparse.y"
            { latA_destruir(((*yyvaluep).node)); }
#line 1611 "latparse.c"
        break;

    case YYSYMBOL_statement_list: /* statement_list  */
#line 130 "latparse.y"
            { latA_destruir(((*yyvaluep).node)); }
#line 1617 "latparse.c"
        break;

    case YYSYMBOL_statement: /* statement  */
#line 130 "latparse.y"
            { latA_destruir(((*yyvaluep).node)); }
#line 1623 "latparse.c"
        break;

    case YYSYMBOL_incdec_statement: /* incdec_statement  */
#line 130 "latparse.y"
            { latA_destruir(((*yyvaluep).node)); }
#line 1629 "latparse.c"
        break;

    case YYSYMBOL_variable_access: /* variable_access  */
#line 130 "latparse.y"
            { latA_destruir(((*yyvaluep).node)); }
#line 1635 "latparse.c"
        break;

    case YYSYMBOL_field_designator: /* field_designator  */
#line 130 "latparse.y"
            { latA_destruir(((*yyvaluep).node)); }
#line 1641 "latparse.c"
        break;

    case YYSYMBOL_global_declaration: /* global_declaration  */
#line 130 "latparse.y"
            { latA_destruir(((*yyvaluep).node)); }
#line 1647 "latparse.c"
        break;

    case YYSYMBOL_declaration: /* declaration  */
#line 130 "latparse.y"
            { latA_destruir(((*yyvaluep).node)); }
#line 1653 "latparse.c"
        break;

    case YYSYMBOL_labeled_statements: /* labeled_statements  */
#line 130 "latparse.y"
            { latA_destruir(((*yyvaluep).node)); }
#line 1659 "latparse.c"
        break;

    case YYSYMBOL_labeled_statement_case: /* labeled_statement_case  */
#line 130 "latparse.y"
            { latA_destruir(((*yyvaluep).node)); }
#line 1665 "latparse.c"
        break;

    case YYSYMBOL_labeled_statement_case_case: /* labeled_statement_case_case  */
#line 130 "latparse.y"
            { latA_destruir(((*yyvaluep).node)); }
#line 1671 "latparse.c"
        break;

    case YYSYMBOL_labeled_statement_default: /* labeled_statement_default  */
#line 130 "latparse.y"
            { latA_destruir(((*yyvaluep).node)); }
#line 1677 "latparse.c"
        break;

    case YYSYMBOL_selection_statement: /* selection_statement  */
#line 130 "latparse.y"
            { latA_destruir(((*yyvaluep).node)); }
#line 1683 "latparse.c"
        break;

    case YYSYMBOL_osi_statements: /* osi_statements  */
#line 130 "latparse.y"
            { latA_destruir(((*yyvaluep).node)); }
#line 1689 "latparse.c"
        break;

    case YYSYMBOL_osi_statement: /* osi_statement  */
#line 130 "latparse.y"
            { latA_destruir(((*yyvaluep).node)); }
#line 1695 "latparse.c"
        break;

    case YYSYMBOL_iteration_statement: /* iteration_statement  */
#line 130 "latparse.y"
            { latA_destruir(((*yyvaluep).node)); }
#line 1701 "latparse.c"
        break;

    case YYSYMBOL_goto_etiqueta: /* goto_etiqueta  */
#line 130 "latparse.y"
            { latA_destruir(((*yyvaluep).node)); }
#line 1707 "latparse.c"
        break;

    case YYSYMBOL_jump_statement: /* jump_statement  */
#line 130 "latparse.y"
            { latA_destruir(((*yyvaluep).node)); }
#line 1713 "latparse.c"
        break;

    case YYSYMBOL_jump_loop: /* jump_loop  */
#line 130 "latparse.y"
            { latA_destruir(((*yyvaluep).node)); }
#line 1719 "latparse.c"
        break;

    case YYSYMBOL_function_definition: /* function_definition  */
#line 130 "latparse.y"
            { latA_destruir(((*yyvaluep).node)); }
#line 1725 "latparse.c"
        break;

    case YYSYMBOL_function_anonymous: /* function_anonymous  */
#line 130 "latparse.y"
            { latA_destruir(((*yyvaluep).node)); }
#line 1731 "latparse.c"
        break;

    case YYSYMBOL_function_call: /* function_call  */
#line 130 "latparse.y"
            { latA_destruir(((*yyvaluep).node)); }
#line 1737 "latparse.c"
        break;

    case YYSYMBOL_argument_expression_list: /* argument_expression_list  */
#line 130 "latparse.y"
            { latA_destruir(((*yyvaluep).node)); }
#line 1743 "latparse.c"
        break;

    case YYSYMBOL_parameter_list: /* parameter_list  */
#line 130 "latparse.y"
            { latA_destruir(((*yyvaluep).node)); }
#line 1749 "latparse.c"
        break;

    case YYSYMBOL_list_new: /* list_new  */
#line 130 "latparse.y"
            { latA_destruir(((*yyvaluep).node)); }
#line 1755 "latparse.c"
        break;

    case YYSYMBOL_list_items: /* list_items  */
#line 130 "latparse.y"
            { latA_destruir(((*yyvaluep).node)); }
#line 1761 "latparse.c"
        break;

    case YYSYMBOL_dict_new: /* dict_new  */
#line 130 "latparse.y"
            { latA_destruir(((*yyvaluep).node)); }
#line 1767 "latparse.c"
        break;

    case YYSYMBOL_dict_items: /* dict_items  */
#line 130 "latparse.y"
            { latA_destruir(((*yyvaluep).node)); }
#line 1773 "latparse.c"
        break;

    case YYSYMBOL_dict_item: /* dict_item  */
#line 130 "latparse.y"
            { latA_destruir(((*yyvaluep).node)); }
#line 1779 "latparse.c"
        break;

    case YYSYMBOL_clase_propiedad: /* clase_propiedad  */
#line 130 "latparse.y"
            { latA_destruir(((*yyvaluep).node)); }
#line 1785 "latparse.c"
        break;

    case YYSYMBOL_clase_propiedades: /* clase_propiedades  */
#line 130 "latparse.y"
            { latA_destruir(((*yyvaluep).node)); }
#line 1791 "latparse.c"
        break;

    case YYSYMBOL_clase_funcion: /* clase_funcion  */
#line 130 "latparse.y"
            { latA_destruir(((*yyvaluep).node)); }
#line 1797 "latparse.c"
        break;

    case YYSYMBOL_clase_funciones: /* clase_funciones  */
#line 130 "latparse.y"
            { latA_destruir(((*yyvaluep).node)); }
#line 1803 "latparse.c"
        break;

    case YYSYMBOL_clase_constructor: /* clase_constructor  */
#line 130 "latparse.y"
            { latA_destruir(((*yyvaluep).node)); }
#line 1809 "latparse.c"
        break;

    case YYSYMBOL_clase_metodo_estatico: /* clase_metodo_estatico  */
#line 130 "latparse.y"
            { latA_destruir(((*yyvaluep).node)); }
#line 1815 "latparse.c"
        break;

    case YYSYMBOL_clase_sentencia: /* clase_sentencia  */
#line 130 "latparse.y"
            { latA_destruir(((*yyvaluep).node)); }
#line 1821 "latparse.c"
        break;

    case YYSYMBOL_clase_sentencias: /* clase_sentencias  */
#line 130 "latparse.y"
            { latA_destruir(((*yyvaluep).node)); }
#line 1827 "latparse.c"
        break;

    case YYSYMBOL_clase_declaracion: /* clase_declaracion  */
#line 130 "latparse.y"
            { latA_destruir(((*yyvaluep).node)); }
#line 1833 "latparse.c"
        break;

      default:
        break;
    }
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}






/*----------.
| yyparse.  |
`----------*/

int
yyparse (ast **root, void *scanner)
{
/* Lookahead token kind.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

/* Location data for the lookahead symbol.  */
static YYLTYPE yyloc_default
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
YYLTYPE yylloc = yyloc_default;

    /* Number of syntax errors so far.  */
    int yynerrs = 0;

    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

    /* The location stack: array, bottom, top.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls = yylsa;
    YYLTYPE *yylsp = yyls;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

  /* The locations where the error started and ended.  */
  YYLTYPE yyerror_range[3];



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */


/* User initialization code.  */
#line 60 "latparse.y"
{ yylloc.file_name = filename; }

#line 1930 "latparse.c"

  yylsp[0] = yylloc;
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yyls1, yysize * YYSIZEOF (*yylsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
        yyls = yyls1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex (&yylval, &yylloc, scanner);
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      yyerror_range[1] = yylloc;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location. */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  yyerror_range[1] = yyloc;
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 5: /* primary_expression: VERDADERO  */
#line 171 "latparse.y"
                { (yyval.node) = latA_logico(1, (yylsp[0]).first_line, (yylsp[0]).first_column);}
#line 2143 "latparse.c"
    break;

  case 6: /* primary_expression: FALSO  */
#line 172 "latparse.y"
            { (yyval.node) = latA_logico(0, (yylsp[0]).first_line, (yylsp[0]).first_column);}
#line 2149 "latparse.c"
    break;

  case 7: /* primary_expression: NULO  */
#line 173 "latparse.y"
           { (yyval.node) = latA_nulo(NULL, (yylsp[0]).first_line, (yylsp[0]).first_column);}
#line 2155 "latparse.c"
    break;

  case 8: /* unary_expression: '-' expression  */
#line 177 "latparse.y"
                               {
        (yyval.node) = latA_nodo(NODO_MENOS_UNARIO, (yyvsp[0].node), NULL, (yylsp[-1]).first_line, (yylsp[-1]).first_column);
        if((yyval.node) == NULL) YYABORT;
    }
#line 2164 "latparse.c"
    break;

  case 9: /* unary_expression: '+' expression  */
#line 181 "latparse.y"
                               {
        (yyval.node) = latA_nodo(NODO_MAS_UNARIO, (yyvsp[0].node), NULL, (yylsp[-1]).first_line, (yylsp[-1]).first_column);
        if((yyval.node) == NULL) YYABORT;
    }
#line 2173 "latparse.c"
    break;

  case 10: /* multiplicative_expression: expression '^' expression  */
#line 188 "latparse.y"
                                {
        (yyval.node) = latA_nodo(NODO_POTENCIA, (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1]).first_line, (yylsp[-1]).first_column);
        if((yyval.node) == NULL) YYABORT;
    }
#line 2182 "latparse.c"
    break;

  case 11: /* multiplicative_expression: expression EXPONENTE expression  */
#line 192 "latparse.y"
                                      {
        (yyval.node) = latA_nodo(NODO_POTENCIA, (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1]).first_line, (yylsp[-1]).first_column);
        if((yyval.node) == NULL) YYABORT;
    }
#line 2191 "latparse.c"
    break;

  case 12: /* multiplicative_expression: expression '*' expression  */
#line 196 "latparse.y"
                                {
        (yyval.node) = latA_nodo(NODO_MULTIPLICACION, (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1]).first_line, (yylsp[-1]).first_column);
        if((yyval.node) == NULL) YYABORT;
    }
#line 2200 "latparse.c"
    break;

  case 13: /* multiplicative_expression: expression '/' expression  */
#line 200 "latparse.y"
                                {
        (yyval.node) = latA_nodo(NODO_DIVISION, (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1]).first_line, (yylsp[-1]).first_column);
        if((yyval.node) == NULL) YYABORT;
    }
#line 2209 "latparse.c"
    break;

  case 14: /* multiplicative_expression: expression '%' expression  */
#line 204 "latparse.y"
                                          {
        (yyval.node) = latA_nodo(NODO_MODULO, (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1]).first_line, (yylsp[-1]).first_column);
        if((yyval.node) == NULL) YYABORT;
    }
#line 2218 "latparse.c"
    break;

  case 15: /* additive_expression: expression '-' expression  */
#line 211 "latparse.y"
                                {
        (yyval.node) = latA_nodo(NODO_RESTA, (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1]).first_line, (yylsp[-1]).first_column);
        if((yyval.node) == NULL) YYABORT;
    }
#line 2227 "latparse.c"
    break;

  case 16: /* additive_expression: expression '+' expression  */
#line 215 "latparse.y"
                                {
        (yyval.node) = latA_nodo(NODO_SUMA, (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1]).first_line, (yylsp[-1]).first_column);
        if((yyval.node) == NULL) YYABORT;
    }
#line 2236 "latparse.c"
    break;

  case 17: /* relational_expression: expression MAYOR_QUE expression  */
#line 222 "latparse.y"
                                      { (yyval.node) = latA_nodo(NODO_MAYOR_QUE, (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1]).first_line, (yylsp[-1]).first_column); }
#line 2242 "latparse.c"
    break;

  case 18: /* relational_expression: expression MENOR_QUE expression  */
#line 223 "latparse.y"
                                      { (yyval.node) = latA_nodo(NODO_MENOR_QUE, (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1]).first_line, (yylsp[-1]).first_column); }
#line 2248 "latparse.c"
    break;

  case 19: /* relational_expression: expression MAYOR_IGUAL expression  */
#line 224 "latparse.y"
                                        { (yyval.node) = latA_nodo(NODO_MAYOR_IGUAL, (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1]).first_line, (yylsp[-1]).first_column); }
#line 2254 "latparse.c"
    break;

  case 20: /* relational_expression: expression MENOR_IGUAL expression  */
#line 225 "latparse.y"
                                        { (yyval.node) = latA_nodo(NODO_MENOR_IGUAL, (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1]).first_line, (yylsp[-1]).first_column); }
#line 2260 "latparse.c"
    break;

  case 21: /* equality_expression: expression DIFERENTE expression  */
#line 229 "latparse.y"
                                      { (yyval.node) = latA_nodo(NODO_DESIGUALDAD, (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1]).first_line, (yylsp[-1]).first_column); }
#line 2266 "latparse.c"
    break;

  case 22: /* equality_expression: expression IGUAL_LOGICO expression  */
#line 230 "latparse.y"
                                         { (yyval.node) = latA_nodo(NODO_IGUALDAD, (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1]).first_line, (yylsp[-1]).first_column); }
#line 2272 "latparse.c"
    break;

  case 23: /* equality_expression: expression REGEX expression  */
#line 231 "latparse.y"
                                  { (yyval.node) = latA_nodo(NODO_REGEX, (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1]).first_line, (yylsp[-1]).first_column); }
#line 2278 "latparse.c"
    break;

  case 24: /* logical_not_expression: '!' expression  */
#line 235 "latparse.y"
                               { (yyval.node) = latA_nodo(NODO_NO, (yyvsp[0].node), NULL, (yylsp[0]).first_line, (yylsp[0]).first_column); }
#line 2284 "latparse.c"
    break;

  case 25: /* logical_and_expression: expression Y_LOGICO expression  */
#line 239 "latparse.y"
                                     { (yyval.node) = latA_nodo(NODO_Y, (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1]).first_line, (yylsp[-1]).first_column); }
#line 2290 "latparse.c"
    break;

  case 26: /* logical_or_expression: expression O_LOGICO expression  */
#line 243 "latparse.y"
                                     { (yyval.node) = latA_nodo(NODO_O, (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1]).first_line, (yylsp[-1]).first_column); }
#line 2296 "latparse.c"
    break;

  case 27: /* ternary_expression: expression '?' expression ':' expression  */
#line 247 "latparse.y"
                                                 { (yyval.node) = latA_si((yyvsp[-4].node), (yyvsp[-2].node), (yyvsp[0].node)); }
#line 2302 "latparse.c"
    break;

  case 28: /* concat_expression: expression CONCATENAR expression  */
#line 251 "latparse.y"
                                       { (yyval.node) = latA_nodo(NODO_CONCATENAR, (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1]).first_line, (yylsp[-1]).first_column); }
#line 2308 "latparse.c"
    break;

  case 29: /* expression: '(' expression ')'  */
#line 255 "latparse.y"
                         { (yyval.node) = (yyvsp[-1].node); }
#line 2314 "latparse.c"
    break;

  case 48: /* expression: VAR_ARGS  */
#line 274 "latparse.y"
               { (yyval.node) = (yyvsp[0].node); (yyval.node)->tipo = NODO_LOAD_VAR_ARGS; }
#line 2320 "latparse.c"
    break;

  case 49: /* expression: SUPER '(' argument_expression_list ')'  */
#line 275 "latparse.y"
                                             { (yyval.node) = latA_super((yyvsp[-1].node), (yylsp[-3]).first_line, (yylsp[-3]).first_column); }
#line 2326 "latparse.c"
    break;

  case 50: /* expression: SUPER '(' ')'  */
#line 276 "latparse.y"
                    { (yyval.node) = latA_super(NULL, (yylsp[-2]).first_line, (yylsp[-2]).first_column); }
#line 2332 "latparse.c"
    break;

  case 51: /* expression: NUEVA IDENTIFICADOR '(' argument_expression_list ')'  */
#line 277 "latparse.y"
                                                           { (yyval.node) = latA_nodo(NODO_FUNCION_LLAMADA, (yyvsp[-3].node), (yyvsp[-1].node), (yylsp[-4]).first_line, (yylsp[-4]).first_column); }
#line 2338 "latparse.c"
    break;

  case 52: /* expression: NUEVA IDENTIFICADOR '(' ')'  */
#line 278 "latparse.y"
                                  { (yyval.node) = latA_nodo(NODO_FUNCION_LLAMADA, (yyvsp[-2].node), NULL, (yylsp[-3]).first_line, (yylsp[-3]).first_column); }
#line 2344 "latparse.c"
    break;

  case 53: /* program: %empty  */
#line 282 "latparse.y"
      { /* empty */
        (yyval.node) = *root = NULL;
    }
#line 2352 "latparse.c"
    break;

  case 54: /* program: statement_list  */
#line 285 "latparse.y"
                     { *root = (yyvsp[0].node); (yyval.node) = NULL; }
#line 2358 "latparse.c"
    break;

  case 55: /* statement_list: statement statement_list  */
#line 289 "latparse.y"
                               {
        if((yyvsp[0].node)){
            (yyval.node) = latA_nodo(NODO_BLOQUE, (yyvsp[-1].node), (yyvsp[0].node), (yylsp[-1]).first_line, (yylsp[-1]).first_column);
        }
    }
#line 2368 "latparse.c"
    break;

  case 56: /* statement_list: statement  */
#line 294 "latparse.y"
                {
        if((yyvsp[0].node)){
          (yyval.node) = latA_nodo(NODO_BLOQUE, (yyvsp[0].node), NULL, (yylsp[0]).first_line, (yylsp[0]).first_column);
        }
    }
#line 2378 "latparse.c"
    break;

  case 68: /* statement: SUPER '(' argument_expression_list ')'  */
#line 313 "latparse.y"
                                             { (yyval.node) = latA_super((yyvsp[-1].node), (yylsp[-3]).first_line, (yylsp[-3]).first_column); }
#line 2384 "latparse.c"
    break;

  case 69: /* statement: SUPER '(' ')'  */
#line 314 "latparse.y"
                    { (yyval.node) = latA_super(NULL, (yylsp[-2]).first_line, (yylsp[-2]).first_column); }
#line 2390 "latparse.c"
    break;

  case 70: /* incdec_statement: variable_access INCREMENTO  */
#line 318 "latparse.y"
                                 { (yyval.node) = latA_nodo(NODO_INC, (yyvsp[-1].node), NULL, (yylsp[0]).first_line, (yylsp[0]).first_column); }
#line 2396 "latparse.c"
    break;

  case 71: /* incdec_statement: variable_access DECREMENTO  */
#line 319 "latparse.y"
                                 { (yyval.node) = latA_nodo(NODO_DEC, (yyvsp[-1].node), NULL, (yylsp[0]).first_line, (yylsp[0]).first_column); }
#line 2402 "latparse.c"
    break;

  case 73: /* variable_access: MI  */
#line 324 "latparse.y"
         { (yyval.node) = latA_var("mi", (yylsp[0]).first_line, (yylsp[0]).first_column, false); }
#line 2408 "latparse.c"
    break;

  case 76: /* field_designator: variable_access ATRIBUTO IDENTIFICADOR  */
#line 330 "latparse.y"
                                             { (yyval.node) = latA_nodo(NODO_ATRIBUTO, (yyvsp[-2].node), (yyvsp[0].node), (yylsp[0]).first_line, (yylsp[0]).first_column); }
#line 2414 "latparse.c"
    break;

  case 77: /* field_designator: variable_access '[' expression ']'  */
#line 331 "latparse.y"
                                         { (yyval.node) = latA_nodo(NODO_LISTA_OBTENER_ELEMENTO, (yyvsp[-1].node), (yyvsp[-3].node), (yylsp[-1]).first_line, (yylsp[-1]).first_column); }
#line 2420 "latparse.c"
    break;

  case 78: /* global_declaration: GLOBAL declaration  */
#line 335 "latparse.y"
                         {
        (yyval.node) = latA_nodo(NODO_GLOBAL, (yyvsp[0].node), NULL, (yylsp[-1]).first_line, (yylsp[-1]).first_column);
    }
#line 2428 "latparse.c"
    break;

  case 79: /* global_declaration: GLOBAL function_definition  */
#line 338 "latparse.y"
                                 {
        (yyval.node) = latA_nodo(NODO_GLOBAL, (yyvsp[0].node), NULL, (yylsp[-1]).first_line, (yylsp[-1]).first_column);
    }
#line 2436 "latparse.c"
    break;

  case 80: /* declaration: variable_access '=' expression  */
#line 344 "latparse.y"
                                     { (yyval.node) = latA_asign((yyvsp[0].node), (yyvsp[-2].node)); }
#line 2442 "latparse.c"
    break;

  case 81: /* declaration: variable_access CONCATENAR_IGUAL expression  */
#line 345 "latparse.y"
                                                  { (yyval.node) = latA_asign(
        (latA_nodo(NODO_CONCATENAR, (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1]).first_line, (yylsp[-1]).first_column)), (yyvsp[-2].node)); }
#line 2449 "latparse.c"
    break;

  case 82: /* declaration: variable_access MAS_IGUAL expression  */
#line 347 "latparse.y"
                                           { (yyval.node) = latA_asign(
        (latA_nodo(NODO_SUMA, (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1]).first_line, (yylsp[-1]).first_column)), (yyvsp[-2].node)); }
#line 2456 "latparse.c"
    break;

  case 83: /* declaration: variable_access MENOS_IGUAL expression  */
#line 349 "latparse.y"
                                             { (yyval.node) = latA_asign(
        (latA_nodo(NODO_RESTA, (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1]).first_line, (yylsp[-1]).first_column)), (yyvsp[-2].node)); }
#line 2463 "latparse.c"
    break;

  case 84: /* declaration: variable_access POR_IGUAL expression  */
#line 351 "latparse.y"
                                           { (yyval.node) = latA_asign(
        (latA_nodo(NODO_MULTIPLICACION, (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1]).first_line, (yylsp[-1]).first_column)), (yyvsp[-2].node)); }
#line 2470 "latparse.c"
    break;

  case 85: /* declaration: variable_access ENTRE_IGUAL expression  */
#line 353 "latparse.y"
                                             { (yyval.node) = latA_asign(
        (latA_nodo(NODO_DIVISION, (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1]).first_line, (yylsp[-1]).first_column)), (yyvsp[-2].node)); }
#line 2477 "latparse.c"
    break;

  case 86: /* declaration: variable_access MODULO_IGUAL expression  */
#line 355 "latparse.y"
                                              { (yyval.node) = latA_asign(
        (latA_nodo(NODO_MODULO, (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-1]).first_line, (yylsp[-1]).first_column)), (yyvsp[-2].node)); }
#line 2484 "latparse.c"
    break;

  case 87: /* declaration: parameter_list '=' argument_expression_list  */
#line 357 "latparse.y"
                                                  { (yyval.node) = latA_asign((yyvsp[0].node), (yyvsp[-2].node)); }
#line 2490 "latparse.c"
    break;

  case 88: /* declaration: variable_access '[' expression ']' '=' expression  */
#line 358 "latparse.y"
                                                        { (yyval.node) = latA_asign_le((yyvsp[0].node), (yyvsp[-5].node), (yyvsp[-3].node)); }
#line 2496 "latparse.c"
    break;

  case 89: /* labeled_statements: labeled_statement_case labeled_statements  */
#line 362 "latparse.y"
                                                {
        (yyval.node) = latA_nodo(NODO_CASOS, (yyvsp[-1].node), (yyvsp[0].node), (yylsp[-1]).first_line, (yylsp[-1]).first_column);
    }
#line 2504 "latparse.c"
    break;

  case 90: /* labeled_statements: labeled_statement_case  */
#line 365 "latparse.y"
                             {
        (yyval.node) = latA_nodo(NODO_CASOS, (yyvsp[0].node), NULL, (yylsp[0]).first_line, (yylsp[0]).first_column);
    }
#line 2512 "latparse.c"
    break;

  case 91: /* labeled_statements: labeled_statement_default  */
#line 368 "latparse.y"
                                {
        (yyval.node) = latA_nodo(NODO_CASOS, (yyvsp[0].node), NULL, (yylsp[0]).first_line, (yylsp[0]).first_column);
    }
#line 2520 "latparse.c"
    break;

  case 92: /* labeled_statement_case: CASO labeled_statement_case_case statement_list  */
#line 374 "latparse.y"
                                                      {
        (yyval.node) = latA_nodo(NODO_CASO, (yyvsp[-1].node), (yyvsp[0].node), (yylsp[-2]).first_line, (yylsp[-2]).first_column);
    }
#line 2528 "latparse.c"
    break;

  case 94: /* labeled_statement_case_case: constant_expression ':' CASO labeled_statement_case_case  */
#line 381 "latparse.y"
                                                               {
        (yyval.node) = latA_nodo(NODO_CASOS, (yyvsp[-3].node), (yyvsp[0].node), (yylsp[-3]).first_line, (yylsp[-3]).first_column);
    }
#line 2536 "latparse.c"
    break;

  case 95: /* labeled_statement_default: DEFECTO ':' statement_list  */
#line 387 "latparse.y"
                                 {
        (yyval.node) = latA_nodo(NODO_DEFECTO, NULL, (yyvsp[0].node), (yylsp[-2]).first_line, (yylsp[-2]).first_column);
    }
#line 2544 "latparse.c"
    break;

  case 96: /* selection_statement: SI expression statement_list FIN  */
#line 393 "latparse.y"
                                       {
        (yyval.node) = latA_si((yyvsp[-2].node), (yyvsp[-1].node), NULL); }
#line 2551 "latparse.c"
    break;

  case 97: /* selection_statement: SI expression statement_list SINO statement_list FIN  */
#line 395 "latparse.y"
                                                           {
        (yyval.node) = latA_si((yyvsp[-4].node), (yyvsp[-3].node), (yyvsp[-1].node)); }
#line 2558 "latparse.c"
    break;

  case 98: /* selection_statement: SI expression statement_list osi_statements FIN  */
#line 397 "latparse.y"
                                                      {
        (yyval.node) = latA_si((yyvsp[-3].node), (yyvsp[-2].node), (yyvsp[-1].node)); }
#line 2565 "latparse.c"
    break;

  case 99: /* selection_statement: ELEGIR expression labeled_statements FIN  */
#line 399 "latparse.y"
                                               {
        (yyval.node) = latA_nodo(NODO_ELEGIR, (yyvsp[-2].node), (yyvsp[-1].node), (yylsp[-3]).first_line, (yylsp[-3]).first_column);
    }
#line 2573 "latparse.c"
    break;

  case 102: /* osi_statement: O_SI expression statement_list  */
#line 410 "latparse.y"
                                     {
          (yyval.node) = latA_si((yyvsp[-1].node), (yyvsp[0].node), NULL);
    }
#line 2581 "latparse.c"
    break;

  case 103: /* osi_statement: O_SI expression statement_list SINO statement_list  */
#line 413 "latparse.y"
                                                         {
          (yyval.node) = latA_si((yyvsp[-3].node), (yyvsp[-2].node), (yyvsp[0].node));
    }
#line 2589 "latparse.c"
    break;

  case 104: /* osi_statement: O_SI expression statement_list osi_statements  */
#line 416 "latparse.y"
                                                    {
          (yyval.node) = latA_si((yyvsp[-2].node), (yyvsp[-1].node), (yyvsp[0].node));
    }
#line 2597 "latparse.c"
    break;

  case 105: /* iteration_statement: MIENTRAS expression statement_list FIN  */
#line 422 "latparse.y"
                                             {
        (yyval.node) = latA_mientras((yyvsp[-2].node), (yyvsp[-1].node)); }
#line 2604 "latparse.c"
    break;

  case 106: /* iteration_statement: REPETIR statement_list HASTA expression  */
#line 424 "latparse.y"
                                              {
        (yyval.node) = latA_hacer((yyvsp[0].node), (yyvsp[-2].node)); }
#line 2611 "latparse.c"
    break;

  case 107: /* iteration_statement: DESDE '(' declaration ';' expression ';' statement ')' statement_list FIN  */
#line 427 "latparse.y"
                            {
        (yyval.node) = latA_desde((yyvsp[-7].node), (yyvsp[-5].node), (yyvsp[-3].node), (yyvsp[-1].node)); }
#line 2618 "latparse.c"
    break;

  case 108: /* iteration_statement: PARA IDENTIFICADOR EN RANGO '(' expression ')' statement_list FIN  */
#line 430 "latparse.y"
                           {
        (yyval.node) = latA_para((yyvsp[-7].node), NULL, (yyvsp[-3].node), NULL, (yyvsp[-1].node));
        if (!(yyval.node)) { yyerror(&(yylsp[-7]), scanner, root, "rango requiere literales finitos y un paso distinto de cero"); YYABORT; }
        }
#line 2627 "latparse.c"
    break;

  case 109: /* iteration_statement: PARA IDENTIFICADOR EN RANGO '(' expression ',' expression ')' statement_list FIN  */
#line 435 "latparse.y"
                           {
        (yyval.node) = latA_para((yyvsp[-9].node), (yyvsp[-5].node), (yyvsp[-3].node), NULL, (yyvsp[-1].node));
        if (!(yyval.node)) { yyerror(&(yylsp[-9]), scanner, root, "rango requiere literales finitos y un paso distinto de cero"); YYABORT; }
        }
#line 2636 "latparse.c"
    break;

  case 110: /* iteration_statement: PARA IDENTIFICADOR EN RANGO '(' expression ',' expression ',' expression ')' statement_list FIN  */
#line 440 "latparse.y"
                           {
        (yyval.node) = latA_para((yyvsp[-11].node), (yyvsp[-7].node), (yyvsp[-5].node), (yyvsp[-3].node), (yyvsp[-1].node));
        if (!(yyval.node)) { yyerror(&(yylsp[-11]), scanner, root, "rango requiere literales finitos y un paso distinto de cero"); YYABORT; }
        }
#line 2645 "latparse.c"
    break;

  case 111: /* goto_etiqueta: IDENTIFICADOR ':'  */
#line 447 "latparse.y"
                        { (yyval.node) = latA_nodo(NODO_ETIQUETA, (yyvsp[-1].node), NULL, (yylsp[-1]).first_line, (yylsp[-1]).first_column); }
#line 2651 "latparse.c"
    break;

  case 112: /* jump_statement: RETORNO expression  */
#line 451 "latparse.y"
                         { (yyval.node) = latA_nodo(NODO_RETORNO, (yyvsp[0].node), NULL, (yylsp[-1]).first_line, (yylsp[-1]).first_column); }
#line 2657 "latparse.c"
    break;

  case 113: /* jump_statement: RETORNO argument_expression_list  */
#line 452 "latparse.y"
                                       { (yyval.node) = latA_nodo(NODO_RETORNO, (yyvsp[0].node), NULL, (yylsp[-1]).first_line, (yylsp[-1]).first_column); }
#line 2663 "latparse.c"
    break;

  case 114: /* jump_statement: IR IDENTIFICADOR  */
#line 453 "latparse.y"
                       { (yyval.node) =  latA_nodo(NODO_IR, (yyvsp[0].node), NULL, (yylsp[-1]).first_line, (yylsp[-1]).first_column); }
#line 2669 "latparse.c"
    break;

  case 115: /* jump_loop: ROMPER  */
#line 457 "latparse.y"
                { (yyval.node) = latA_nodo(NODO_ROMPER, NULL, NULL, (yylsp[0]).first_line, (yylsp[0]).first_column); }
#line 2675 "latparse.c"
    break;

  case 116: /* jump_loop: CONTINUAR  */
#line 458 "latparse.y"
                { (yyval.node) = latA_nodo(NODO_CONTINUAR, NULL, NULL, (yylsp[0]).first_line, (yylsp[0]).first_column); }
#line 2681 "latparse.c"
    break;

  case 117: /* function_definition: FUNCION IDENTIFICADOR '(' parameter_list ')' statement_list FIN  */
#line 462 "latparse.y"
                                                                      {
        (yyval.node) = latA_funcion((yyvsp[-5].node), (yyvsp[-3].node), (yyvsp[-1].node), (yylsp[-5]).first_line, (yylsp[-5]).first_column);
    }
#line 2689 "latparse.c"
    break;

  case 118: /* function_anonymous: FUNCION '(' parameter_list ')' statement_list FIN  */
#line 468 "latparse.y"
                                                        {
        (yyval.node) = latA_funcion(latA_var("anonima", (yylsp[-5]).first_line, (yylsp[-5]).first_column, false), (yyvsp[-3].node), (yyvsp[-1].node), (yylsp[-5]).first_line, (yylsp[-5]).first_column);
    }
#line 2697 "latparse.c"
    break;

  case 119: /* function_call: variable_access '(' argument_expression_list ')'  */
#line 474 "latparse.y"
                                                       { (yyval.node) = latA_nodo(NODO_FUNCION_LLAMADA, (yyvsp[-3].node), (yyvsp[-1].node), (yylsp[-3]).first_line, (yylsp[-3]).first_column); }
#line 2703 "latparse.c"
    break;

  case 120: /* argument_expression_list: %empty  */
#line 478 "latparse.y"
                  { (yyval.node) = NULL; }
#line 2709 "latparse.c"
    break;

  case 121: /* argument_expression_list: expression  */
#line 479 "latparse.y"
                 { (yyval.node) = latA_nodo(NODO_FUNCION_ARGUMENTOS, (yyvsp[0].node), NULL, (yylsp[0]).first_line, (yylsp[0]).first_column); }
#line 2715 "latparse.c"
    break;

  case 122: /* argument_expression_list: expression ',' argument_expression_list  */
#line 480 "latparse.y"
                                              {
        (yyval.node) = latA_nodo(NODO_FUNCION_ARGUMENTOS, (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-2]).first_line, (yylsp[-2]).first_column);
    }
#line 2723 "latparse.c"
    break;

  case 123: /* parameter_list: %empty  */
#line 486 "latparse.y"
                  { (yyval.node) = NULL; }
#line 2729 "latparse.c"
    break;

  case 124: /* parameter_list: IDENTIFICADOR  */
#line 487 "latparse.y"
                    { (yyval.node) = latA_nodo(NODO_FUNCION_PARAMETROS, (yyvsp[0].node), NULL, (yylsp[0]).first_line, (yylsp[0]).first_column); }
#line 2735 "latparse.c"
    break;

  case 125: /* parameter_list: VAR_ARGS  */
#line 488 "latparse.y"
               { (yyval.node) = latA_nodo(NODO_FUNCION_PARAMETROS, (yyvsp[0].node), NULL, 0, 0); }
#line 2741 "latparse.c"
    break;

  case 126: /* parameter_list: parameter_list ',' IDENTIFICADOR  */
#line 489 "latparse.y"
                                       {
        if((yyvsp[-2].node)->izq->tipo == NODO_VAR_ARGS){
            yyerror(&(yylsp[0]), scanner, root, "El parámetro ... debe ser el último");
            latA_destruir((yyvsp[-2].node));
            latA_destruir((yyvsp[0].node));
            YYABORT;
        }
        (yyval.node) = latA_nodo(NODO_FUNCION_PARAMETROS, (yyvsp[0].node), (yyvsp[-2].node), (yylsp[0]).first_line, (yylsp[0]).first_column); }
#line 2754 "latparse.c"
    break;

  case 127: /* parameter_list: parameter_list ',' VAR_ARGS  */
#line 497 "latparse.y"
                                  {
        if ((yyvsp[-2].node)->izq->tipo == NODO_VAR_ARGS) {
            yyerror(&(yylsp[0]), scanner, root, "El parámetro ... debe aparecer una sola vez");
            latA_destruir((yyvsp[-2].node));
            latA_destruir((yyvsp[0].node));
            YYABORT;
        }
        (yyval.node) = latA_nodo(NODO_FUNCION_PARAMETROS, (yyvsp[0].node), (yyvsp[-2].node), (yylsp[0]).first_line, (yylsp[0]).first_column);
    }
#line 2768 "latparse.c"
    break;

  case 128: /* list_new: '[' list_items ']'  */
#line 509 "latparse.y"
                         { (yyval.node) = latA_nodo(NODO_LISTA, (yyvsp[-1].node), NULL, (yylsp[-2]).first_line, (yylsp[-2]).first_column); }
#line 2774 "latparse.c"
    break;

  case 129: /* list_items: %empty  */
#line 513 "latparse.y"
                  { (yyval.node) = NULL; }
#line 2780 "latparse.c"
    break;

  case 130: /* list_items: expression  */
#line 514 "latparse.y"
                 { (yyval.node) = latA_nodo(NODO_LISTA_AGREGAR_ELEMENTO, (yyvsp[0].node), NULL, (yylsp[0]).first_line, (yylsp[0]).first_column); }
#line 2786 "latparse.c"
    break;

  case 131: /* list_items: expression ',' list_items  */
#line 515 "latparse.y"
                                {
        if((yyvsp[-2].node)->tipo == NODO_LOAD_VAR_ARGS){
            yyerror(&(yylsp[-2]), scanner, root, "La expansión ... debe cerrar la lista");
            latA_destruir((yyvsp[-2].node));
            latA_destruir((yyvsp[0].node));
            YYABORT;
        }
        (yyval.node) = latA_nodo(NODO_LISTA_AGREGAR_ELEMENTO, (yyvsp[-2].node), (yyvsp[0].node), (yylsp[0]).first_line, (yylsp[0]).first_column);
    }
#line 2800 "latparse.c"
    break;

  case 132: /* dict_new: '{' dict_items '}'  */
#line 527 "latparse.y"
                         { (yyval.node) = latA_nodo(NODO_DICCIONARIO, (yyvsp[-1].node), NULL, (yylsp[-1]).first_line, (yylsp[-1]).first_column); }
#line 2806 "latparse.c"
    break;

  case 133: /* dict_items: %empty  */
#line 531 "latparse.y"
                  { (yyval.node) = NULL; }
#line 2812 "latparse.c"
    break;

  case 134: /* dict_items: dict_item  */
#line 532 "latparse.y"
                { (yyval.node) = latA_nodo(NODO_DICC_AGREGAR_ELEMENTO, (yyvsp[0].node), NULL, (yylsp[0]).first_line, (yylsp[0]).first_column); }
#line 2818 "latparse.c"
    break;

  case 135: /* dict_items: dict_items ',' dict_item  */
#line 533 "latparse.y"
                               { (yyval.node) = latA_nodo(NODO_DICC_AGREGAR_ELEMENTO, (yyvsp[0].node), (yyvsp[-2].node), (yylsp[0]).first_line, (yylsp[0]).first_column); }
#line 2824 "latparse.c"
    break;

  case 136: /* dict_item: %empty  */
#line 537 "latparse.y"
  { /* empty */ (yyval.node) = NULL; }
#line 2830 "latparse.c"
    break;

  case 137: /* dict_item: IDENTIFICADOR ':' expression  */
#line 538 "latparse.y"
                               {
    /* Ajustar sintaxis para claves de diccionario sin comillas:
       { clave: valor } -> clave se trata como cadena literal. */
    ast *clave_id = (yyvsp[-2].node);
    ast *clave_str = latA_cadena(clave_id->valor->val.cadena, (yylsp[-2]).first_line, (yylsp[-2]).first_column);
    /* Liberar el identificador original para evitar fuga menor */
    latA_destruir(clave_id);
    (yyval.node) = latA_nodo(NODO_DICC_ELEMENTO, clave_str, (yyvsp[0].node), (yylsp[-2]).first_line, (yylsp[-2]).first_column);
  }
#line 2844 "latparse.c"
    break;

  case 138: /* dict_item: expression ':' expression  */
#line 547 "latparse.y"
                            { (yyval.node) = latA_nodo(NODO_DICC_ELEMENTO, (yyvsp[-2].node), (yyvsp[0].node), (yylsp[-2]).first_line, (yylsp[-2]).first_column); }
#line 2850 "latparse.c"
    break;

  case 139: /* clase_propiedad: IDENTIFICADOR '=' expression  */
#line 551 "latparse.y"
                                   { (yyval.node) = latA_asign((yyvsp[0].node), (yyvsp[-2].node)); }
#line 2856 "latparse.c"
    break;

  case 140: /* clase_propiedades: clase_propiedad clase_propiedades  */
#line 555 "latparse.y"
                                        {
        if((yyvsp[0].node)){
            (yyval.node) = latA_nodo(NODO_BLOQUE, (yyvsp[-1].node), (yyvsp[0].node), (yylsp[-1]).first_line, (yylsp[-1]).first_column);
        }
    }
#line 2866 "latparse.c"
    break;

  case 141: /* clase_propiedades: clase_propiedad  */
#line 560 "latparse.y"
                      {
        if((yyvsp[0].node)){
          (yyval.node) = latA_nodo(NODO_BLOQUE, (yyvsp[0].node), NULL, (yylsp[0]).first_line, (yylsp[0]).first_column);
        }
    }
#line 2876 "latparse.c"
    break;

  case 142: /* clase_funcion: FUNCION IDENTIFICADOR '(' parameter_list ')' statement_list FIN  */
#line 568 "latparse.y"
                                                                      {
        (yyval.node) = latA_funcion((yyvsp[-5].node), (yyvsp[-3].node), (yyvsp[-1].node), (yylsp[-5]).first_line, (yylsp[-5]).first_column);
    }
#line 2884 "latparse.c"
    break;

  case 143: /* clase_funciones: clase_funcion clase_funciones  */
#line 574 "latparse.y"
                                    {
        if((yyvsp[0].node)){
            (yyval.node) = latA_nodo(NODO_BLOQUE, (yyvsp[-1].node), (yyvsp[0].node), (yylsp[-1]).first_line, (yylsp[-1]).first_column);
        }
    }
#line 2894 "latparse.c"
    break;

  case 144: /* clase_funciones: clase_funcion  */
#line 579 "latparse.y"
                    {
        if((yyvsp[0].node)){
          (yyval.node) = latA_nodo(NODO_BLOQUE, (yyvsp[0].node), NULL, (yylsp[0]).first_line, (yylsp[0]).first_column);
        }
    }
#line 2904 "latparse.c"
    break;

  case 145: /* clase_constructor: CONSTRUCTOR '(' parameter_list ')' statement_list FIN  */
#line 587 "latparse.y"
                                                            {
        (yyval.node) = latA_funcion(latA_var("constructor", (yylsp[-5]).first_line, (yylsp[-5]).first_column, false), (yyvsp[-3].node), (yyvsp[-1].node), (yylsp[-5]).first_line, (yylsp[-5]).first_column);
    }
#line 2912 "latparse.c"
    break;

  case 146: /* clase_constructor: CONSTRUCTOR '(' ')' statement_list FIN  */
#line 590 "latparse.y"
                                             {
        (yyval.node) = latA_funcion(latA_var("constructor", (yylsp[-4]).first_line, (yylsp[-4]).first_column, false), NULL, (yyvsp[-1].node), (yylsp[-4]).first_line, (yylsp[-4]).first_column);
    }
#line 2920 "latparse.c"
    break;

  case 147: /* clase_metodo_estatico: ESTATICO FUNCION IDENTIFICADOR '(' parameter_list ')' statement_list FIN  */
#line 596 "latparse.y"
                                                                               {
        (yyval.node) = latA_funcion((yyvsp[-5].node), (yyvsp[-3].node), (yyvsp[-1].node), (yylsp[-5]).first_line, (yylsp[-5]).first_column);
    }
#line 2928 "latparse.c"
    break;

  case 148: /* clase_metodo_estatico: ESTATICO FUNCION IDENTIFICADOR '(' ')' statement_list FIN  */
#line 599 "latparse.y"
                                                                {
        (yyval.node) = latA_funcion((yyvsp[-4].node), NULL, (yyvsp[-1].node), (yylsp[-4]).first_line, (yylsp[-4]).first_column);
    }
#line 2936 "latparse.c"
    break;

  case 154: /* clase_sentencia: RETORNO expression  */
#line 610 "latparse.y"
                         { (yyval.node) = latA_nodo(NODO_RETORNO, (yyvsp[0].node), NULL, (yylsp[-1]).first_line, (yylsp[-1]).first_column); }
#line 2942 "latparse.c"
    break;

  case 155: /* clase_sentencia: RETORNO argument_expression_list  */
#line 611 "latparse.y"
                                       { (yyval.node) = latA_nodo(NODO_RETORNO, (yyvsp[0].node), NULL, (yylsp[-1]).first_line, (yylsp[-1]).first_column); }
#line 2948 "latparse.c"
    break;

  case 156: /* clase_sentencias: clase_sentencia clase_sentencias  */
#line 615 "latparse.y"
                                       {
        if((yyvsp[0].node)){
            (yyval.node) = latA_nodo(NODO_BLOQUE, (yyvsp[-1].node), (yyvsp[0].node), (yylsp[-1]).first_line, (yylsp[-1]).first_column);
        }
    }
#line 2958 "latparse.c"
    break;

  case 157: /* clase_sentencias: clase_sentencia  */
#line 620 "latparse.y"
                      {
        if((yyvsp[0].node)){
          (yyval.node) = latA_nodo(NODO_BLOQUE, (yyvsp[0].node), NULL, (yylsp[0]).first_line, (yylsp[0]).first_column);
        }
    }
#line 2968 "latparse.c"
    break;

  case 158: /* clase_declaracion: CLASE IDENTIFICADOR HEREDA IDENTIFICADOR clase_sentencias FIN  */
#line 628 "latparse.y"
                                                                    {
        (yyval.node) = latA_clase((yyvsp[-4].node), (yyvsp[-2].node), (yyvsp[-1].node), (yylsp[-4]).first_line, (yylsp[-4]).first_column);
    }
#line 2976 "latparse.c"
    break;

  case 159: /* clase_declaracion: CLASE IDENTIFICADOR clase_sentencias FIN  */
#line 631 "latparse.y"
                                               {
        (yyval.node) = latA_clase((yyvsp[-2].node), NULL, (yyvsp[-1].node), (yylsp[-2]).first_line, (yylsp[-2]).first_column);
    }
#line 2984 "latparse.c"
    break;


#line 2988 "latparse.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (&yylloc, root, scanner, YY_("syntax error"));
    }

  yyerror_range[1] = yylloc;
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, &yylloc, root, scanner);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, yylsp, root, scanner);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  ++yylsp;
  YYLLOC_DEFAULT (*yylsp, yyerror_range, 2);

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (&yylloc, root, scanner, YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc, root, scanner);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, yylsp, root, scanner);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 636 "latparse.y"


//se define para analisis sintactico (bison)
int yyerror(struct YYLTYPE *yylloc_param, void *scanner, struct ast **root,
const char *s) {
  if (!parse_silent) {
      const char *mensaje = !strcmp(s, "syntax error") ? "Error de sintaxis" : s;
      fprintf(stderr, LAT_ERROR_FMT, filename ? filename : "<entrada>",
              yylloc_param->first_line > 0 ? yylloc_param->first_line : 1,
              yylloc_param->first_column > 0 ? yylloc_param->first_column : 1,
              mensaje);
  }
  return 0;
}
