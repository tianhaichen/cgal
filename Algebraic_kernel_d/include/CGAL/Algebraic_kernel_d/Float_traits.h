// TODO: Add licence
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $URL:$
// $Id: $
// 
//
// Author(s)     : 
//
// ============================================================================

// TODO: Some comments are original EXACUS comments and aren't adapted. So
//         they may be wrong now.

#ifndef CGAL_ALGEBRAIC_KERNEL_D_FLOAT_TRAITS_H
#define CGAL_ALGEBRAIC_KERNEL_D_FLOAT_TRAITS_H

#include <CGAL/basic.h>


CGAL_BEGIN_NAMESPACE

namespace CGALi {
    
    template< class Type >
    class Float_traits {
      public:
        
        typedef Null_functor    Get_mantissa;
        typedef Null_functor    Get_exponent;  
        typedef Null_functor    Mul_by_pow_of_2;

    };
    
#ifdef CGAL_USE_LEDA

    // Specialization for leda_bigfloat
    template<>
    class Float_traits< leda_bigfloat > {
      public:
      
        struct Get_mantissa
            : public Unary_function< leda_bigfloat, leda_integer > {
            leda_integer operator()( const leda_bigfloat& x ) const {
                return x.get_significant();                
            }
        };
        
        struct Get_exponent
            : public Unary_function< leda_bigfloat, long > {
            long operator()( const leda_bigfloat& x ) const {
                return x.get_exponent().to_long();                
            }
        };

        struct Mul_by_pow_of_2
            : public Binary_function< leda_bigfloat, leda_integer, 
                                      leda_bigfloat> {
            leda_bigfloat operator()( const leda_bigfloat& a, 
                                      const leda_integer&  e ) const {
                return leda_bigfloat(a.get_significant(), a.get_exponent()+e);
            }
        };
    };

#endif    
    
#ifdef CGAL_USE_CORE

    // Specialization for CORE::BigFloat
    template<>
    class Float_traits< CORE::BigFloat > {
      public:
      
        struct Get_mantissa
            : public Unary_function< CORE::BigFloat, CORE::BigInt > {
            CORE::BigInt operator()( const CORE::BigFloat& x ) const {
                return x.m();
            }
        };
        
        struct Get_exponent
            : public Unary_function< CORE::BigFloat, long > {
            long operator()( const CORE::BigFloat& x ) const {
                return 14*x.exp(); // The basis is 8092                 
            }
        };

        struct Mul_by_pow_of_2
            : public Binary_function< CORE::BigFloat, CORE::BigInt, 
                                      CORE::BigFloat> {
            CORE::BigFloat operator()( const CORE::BigFloat& a, 
                                       const CORE::BigInt&   e ) const {
                return a*CORE::BigFloat::exp2(e.intValue());
            }
        };

    };

#endif    
    
} //namespace CGALi

CGAL_END_NAMESPACE

#endif // CGAL_ALGEBRAIC_KERNEL_D_FLOAT_TRAITS_H
