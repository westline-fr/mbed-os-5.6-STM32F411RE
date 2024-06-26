/**************************************************************************//**
 * @file     cmsis_compiler.h
 * @brief    CMSIS compiler generic header file
 * @version  V5.0.2
 * @date     13. February 2017
 ******************************************************************************/
/*
 * Copyright (c) 2009-2017 ARM Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __CMSIS_COMPILER_H
#define __CMSIS_COMPILER_H

#include <stdint.h>

/*
 * ARM Compiler 4/5
 */
#if   defined ( __CC_ARM )
  #include "cmsis_armcc.h"


/*
 * ARM Compiler 6 (armclang)
 */
#elif defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  #include "cmsis_armclang.h"


/*
 * GNU Compiler
 */
#elif defined ( __GNUC__ )
  #include "cmsis_gcc.h"


/*
 * IAR Compiler
 */
#elif defined ( __ICCARM__ )

  #ifndef   __ASM
    #define __ASM                                  __asm
  #endif
  #ifndef   __INLINE
    #define __INLINE                               inline
  #endif
  #ifndef   __STATIC_INLINE
    #define __STATIC_INLINE                        static inline
  #endif

  #include <cmsis_iar.h>

  /* CMSIS compiler control architecture macros */
  #if (__CORE__ == __ARM6M__) || (__CORE__ == __ARM6SM__)
    #ifndef __ARM_ARCH_6M__
      #define __ARM_ARCH_6M__                      1
    #endif
  #elif (__CORE__ == __ARM7M__)
    #ifndef __ARM_ARCH_7M__
      #define __ARM_ARCH_7M__                      1
    #endif
  #elif (__CORE__ == __ARM7EM__)
    #ifndef __ARM_ARCH_7EM__
      #define __ARM_ARCH_7EM__                     1
    #endif
  #elif (__CORE__ == __ARM8M_BASELINE__)
    #ifndef __ARM_ARCH_8M_BASE__
      #define __ARM_ARCH_8M_BASE__                 1
    #endif
  #elif (__CORE__ == __ARM8M_MAINLINE__)
    #ifndef __ARM_ARCH_8M_MAIN__
      #define __ARM_ARCH_8M_MAIN__                 1
    #endif
  #endif

  // IAR version 7.8.1 and earlier do not include __ALIGNED
  #ifndef __ALIGNED
  #define __ALIGNED(x) __attribute__((aligned(x)))
  #endif

  #ifndef   __NO_RETURN
    #define __NO_RETURN                            __noreturn
  #endif
  #ifndef   __USED
    #define __USED                                 __root
  #endif
  #ifndef   __WEAK
    #define __WEAK                                 __weak
  #endif
  #ifndef   __PACKED
    #define __PACKED                               __packed
  #endif
  #ifndef   __PACKED_STRUCT
    #define __PACKED_STRUCT                        __packed struct
  #endif
  #ifndef   __UNALIGNED_UINT32        /* deprecated */
    __packed struct T_UINT32 { uint32_t v; };
    #define __UNALIGNED_UINT32(x)                  (((struct T_UINT32 *)(x))->v)
  #endif
  #ifndef   __UNALIGNED_UINT16_WRITE
    __PACKED_STRUCT T_UINT16_WRITE { uint16_t v; };
    #define __UNALIGNED_UINT16_WRITE(addr, val)    (void)((((struct T_UINT16_WRITE *)(void *)(addr))->v) = (val))
  #endif
  #ifndef   __UNALIGNED_UINT16_READ
    __PACKED_STRUCT T_UINT16_READ { uint16_t v; };
    #define __UNALIGNED_UINT16_READ(addr)          (((const struct T_UINT16_READ *)(const void *)(addr))->v)
  #endif
  #ifndef   __UNALIGNED_UINT32_WRITE
    __PACKED_STRUCT T_UINT32_WRITE { uint32_t v; };
    #define __UNALIGNED_UINT32_WRITE(addr, val)    (void)((((struct T_UINT32_WRITE *)(void *)(addr))->v) = (val))
  #endif
  #ifndef   __UNALIGNED_UINT32_READ
    __PACKED_STRUCT T_UINT32_READ { uint32_t v; };
    #define __UNALIGNED_UINT32_READ(addr)          (((const struct T_UINT32_READ *)(const void *)(addr))->v)
  #endif
  #ifndef   __ALIGNED
    #warning No compiler specific solution for __ALIGNED. __ALIGNED is ignored.
    #define __ALIGNED(x)
  #endif


/*
 * TI ARM Compiler
 */
#elif defined ( __TI_ARM__ )
  #include <cmsis_ccs.h>

  #ifndef   __ASM
    #define __ASM                                  __asm
  #endif
  #ifndef   __INLINE
    #define __INLINE                               inline
  #endif
  #ifndef   __STATIC_INLINE
    #define __STATIC_INLINE                        static inline
  #endif
  #ifndef   __NO_RETURN
    #define __NO_RETURN                            __attribute__((noreturn))
  #endif
  #ifndef   __USED
    #define __USED                                 __attribute__((used))
  #endif
  #ifndef   __WEAK
    #define __WEAK                                 __attribute__((weak))
  #endif
  #ifndef   __PACKED
    #define __PACKED                               __attribute__((packed))
  #endif
  #ifndef   __PACKED_STRUCT
    #define __PACKED_STRUCT                        struct __attribute__((packed))
  #endif
  #ifndef   __UNALIGNED_UINT32        /* deprecated */
    struct __attribute__((packed)) T_UINT32 { uint32_t v; };
    #define __UNALIGNED_UINT32(x)                  (((struct T_UINT32 *)(x))->v)
  #endif
  #ifndef   __UNALIGNED_UINT16_WRITE
    __PACKED_STRUCT T_UINT16_WRITE { uint16_t v; };
    #define __UNALIGNED_UINT16_WRITE(addr, val)    (void)((((struct T_UINT16_WRITE *)(void*)(addr))->v) = (val))
  #endif
  #ifndef   __UNALIGNED_UINT16_READ
    __PACKED_STRUCT T_UINT16_READ { uint16_t v; };
    #define __UNALIGNED_UINT16_READ(addr)          (((const struct T_UINT16_READ *)(const void *)(addr))->v)
  #endif
  #ifndef   __UNALIGNED_UINT32_WRITE
    __PACKED_STRUCT T_UINT32_WRITE { uint32_t v; };
    #define __UNALIGNED_UINT32_WRITE(addr, val)    (void)((((struct T_UINT32_WRITE *)(void *)(addr))->v) = (val))
  #endif
  #ifndef   __UNALIGNED_UINT32_READ
    __PACKED_STRUCT T_UINT32_READ { uint32_t v; };
    #define __UNALIGNED_UINT32_READ(addr)          (((const struct T_UINT32_READ *)(const void *)(addr))->v)
  #endif
  #ifndef   __ALIGNED
    #define __ALIGNED(x)                           __attribute__((aligned(x)))
  #endif


/*
 * TASKING Compiler
 */
#elif defined ( __TASKING__ )
  /*
   * The CMSIS functions have been implemented as intrinsics in the compiler.
   * Please use "carm -?i" to get an up to date list of all intrinsics,
   * Including the CMSIS ones.
   */

  #ifndef   __ASM
    #define __ASM                                  __asm
  #endif
  #ifndef   __INLINE
    #define __INLINE                               inline
  #endif
  #ifndef   __STATIC_INLINE
    #define __STATIC_INLINE                        static inline
  #endif
  #ifndef   __NO_RETURN
    #define __NO_RETURN                            __attribute__((noreturn))
  #endif
  #ifndef   __USED
    #define __USED                                 __attribute__((used))
  #endif
  #ifndef   __WEAK
    #define __WEAK                                 __attribute__((weak))
  #endif
  #ifndef   __PACKED
    #define __PACKED                               __packed__
  #endif
  #ifndef   __PACKED_STRUCT
    #define __PACKED_STRUCT                        struct __packed__
  #endif
  #ifndef   __UNALIGNED_UINT32        /* deprecated */
    struct __packed__ T_UINT32 { uint32_t v; };
    #define __UNALIGNED_UINT32(x)                  (((struct T_UINT32 *)(x))->v)
  #endif
  #ifndef   __UNALIGNED_UINT16_WRITE
    __PACKED_STRUCT T_UINT16_WRITE { uint16_t v; };
    #define __UNALIGNED_UINT16_WRITE(addr, val)    (void)((((struct T_UINT16_WRITE *)(void *)(addr))->v) = (val))
  #endif
  #ifndef   __UNALIGNED_UINT16_READ
    __PACKED_STRUCT T_UINT16_READ { uint16_t v; };
    #define __UNALIGNED_UINT16_READ(addr)          (((const struct T_UINT16_READ *)(const void *)(addr))->v)
  #endif
  #ifndef   __UNALIGNED_UINT32_WRITE
    __PACKED_STRUCT T_UINT32_WRITE { uint32_t v; };
    #define __UNALIGNED_UINT32_WRITE(addr, val)    (void)((((struct T_UINT32_WRITE *)(void *)(addr))->v) = (val))
  #endif
  #ifndef   __UNALIGNED_UINT32_READ
    __PACKED_STRUCT T_UINT32_READ { uint32_t v; };
    #define __UNALIGNED_UINT32_READ(addr)          (((const struct T_UINT32_READ *)(const void *)(addr))->v)
  #endif
  #ifndef   __ALIGNED
    #define __ALIGNED(x)              __align(x)
  #endif


/*
 * COSMIC Compiler
 */
#elif defined ( __CSMC__ )
   #include <cmsis_csm.h>

 #ifndef   __ASM
    #define __ASM                                  _asm
  #endif
  #ifndef   __INLINE
    #define __INLINE                               inline
  #endif
  #ifndef   __STATIC_INLINE
    #define __STATIC_INLINE                        static inline
  #endif
  #ifndef   __NO_RETURN
    // NO RETURN is automatically detected hence no warning here
    #define __NO_RETURN
  #endif
  #ifndef   __USED
    #warning No compiler specific solution for __USED. __USED is ignored.
    #define __USED
  #endif
  #ifndef   __WEAK
    #define __WEAK                                 __weak
  #endif
  #ifndef   __PACKED
    #define __PACKED                               @packed
  #endif
  #ifndef   __PACKED_STRUCT
    #define __PACKED_STRUCT                        @packed struct
  #endif
  #ifndef   __UNALIGNED_UINT32        /* deprecated */
    @packed struct T_UINT32 { uint32_t v; };
    #define __UNALIGNED_UINT32(x)                  (((struct T_UINT32 *)(x))->v)
  #endif
  #ifndef   __UNALIGNED_UINT16_WRITE
    __PACKED_STRUCT T_UINT16_WRITE { uint16_t v; };
    #define __UNALIGNED_UINT16_WRITE(addr, val)    (void)((((struct T_UINT16_WRITE *)(void *)(addr))->v) = (val))
  #endif
  #ifndef   __UNALIGNED_UINT16_READ
    __PACKED_STRUCT T_UINT16_READ { uint16_t v; };
    #define __UNALIGNED_UINT16_READ(addr)          (((const struct T_UINT16_READ *)(const void *)(addr))->v)
  #endif
  #ifndef   __UNALIGNED_UINT32_WRITE
    __PACKED_STRUCT T_UINT32_WRITE { uint32_t v; };
    #define __UNALIGNED_UINT32_WRITE(addr, val)    (void)((((struct T_UINT32_WRITE *)(void *)(addr))->v) = (val))
  #endif
  #ifndef   __UNALIGNED_UINT32_READ
    __PACKED_STRUCT T_UINT32_READ { uint32_t v; };
    #define __UNALIGNED_UINT32_READ(addr)          (((const struct T_UINT32_READ *)(const void *)(addr))->v)
  #endif
  #ifndef   __ALIGNED
    #warning No compiler specific solution for __ALIGNED. __ALIGNED is ignored.
    #define __ALIGNED(x)
  #endif


#else
  #error Unknown compiler.
#endif


#endif /* __CMSIS_COMPILER_H */

