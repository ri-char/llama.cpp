#ifndef RVV_MATHFUN_FP32_H
#define RVV_MATHFUN_FP32_H

#ifdef __riscv_v

#include <riscv_vector.h>

#define c_exp_hi 88.3762626647949f
#define c_exp_lo -88.3762626647949f

#define c_cephes_LOG2EF 1.44269504088896341
#define c_cephes_exp_C1 0.693359375
#define c_cephes_exp_C2 -2.12194440e-4

#define c_cephes_exp_p0 1.9875691500E-4
#define c_cephes_exp_p1 1.3981999507E-3
#define c_cephes_exp_p2 8.3334519073E-3
#define c_cephes_exp_p3 4.1665795894E-2
#define c_cephes_exp_p4 1.6666665459E-1
#define c_cephes_exp_p5 5.0000001201E-1

#define _RVV_FLOAT32_EXP_OP(LMUL, MLEN)                                                                        \
    static inline vfloat32m##LMUL##_t exp_ps_vfloat32m##LMUL(vfloat32m##LMUL##_t x, size_t vl)                 \
    {                                                                                                          \
        vfloat32m##LMUL##_t tmp, fx;                                                                           \
                                                                                                               \
        x = __riscv_vfmin_vf_f32m##LMUL(x, c_exp_hi, vl);                                                      \
        x = __riscv_vfmax_vf_f32m##LMUL(x, c_exp_lo, vl);                                                      \
                                                                                                               \
        /* express exp(x) as exp(g + n*log(2)) */                                                              \
        fx = __riscv_vfmacc_vf_f32m##LMUL(__riscv_vfmv_v_f_f32m##LMUL(0.5f, vl), c_cephes_LOG2EF, x, vl);      \
                                                                                                               \
        /* perform a floorf */                                                                                 \
        tmp = __riscv_vfcvt_f_x_v_f32m##LMUL(__riscv_vfcvt_x_f_v_i32m##LMUL(fx, vl), vl);                      \
                                                                                                               \
        /* if greater, substract 1 */                                                                          \
        vbool##MLEN##_t mask = __riscv_vmfgt_vv_f32m##LMUL##_b##MLEN(tmp, fx, vl);                             \
        fx = __riscv_vfsub_vf_f32m##LMUL##_m(mask, tmp, 1.f, vl);                                              \
                                                                                                               \
        tmp = __riscv_vfmul_vf_f32m##LMUL(fx, c_cephes_exp_C1, vl);                                            \
        vfloat32m##LMUL##_t z = __riscv_vfmul_vf_f32m##LMUL(fx, c_cephes_exp_C2, vl);                          \
        x = __riscv_vfsub_vv_f32m##LMUL(x, tmp, vl);                                                           \
        x = __riscv_vfsub_vv_f32m##LMUL(x, z, vl);                                                             \
                                                                                                               \
        vfloat32m##LMUL##_t y = __riscv_vfmul_vf_f32m##LMUL(x, c_cephes_exp_p0, vl);                           \
        z = __riscv_vfmul_vv_f32m##LMUL(x, x, vl);                                                             \
                                                                                                               \
        y = __riscv_vfadd_vf_f32m##LMUL(y, c_cephes_exp_p1, vl);                                               \
        y = __riscv_vfmul_vv_f32m##LMUL(y, x, vl);                                                             \
        y = __riscv_vfadd_vf_f32m##LMUL(y, c_cephes_exp_p2, vl);                                               \
        y = __riscv_vfmul_vv_f32m##LMUL(y, x, vl);                                                             \
        y = __riscv_vfadd_vf_f32m##LMUL(y, c_cephes_exp_p3, vl);                                               \
        y = __riscv_vfmul_vv_f32m##LMUL(y, x, vl);                                                             \
        y = __riscv_vfadd_vf_f32m##LMUL(y, c_cephes_exp_p4, vl);                                               \
        y = __riscv_vfmul_vv_f32m##LMUL(y, x, vl);                                                             \
        y = __riscv_vfadd_vf_f32m##LMUL(y, c_cephes_exp_p5, vl);                                               \
                                                                                                               \
        y = __riscv_vfmul_vv_f32m##LMUL(y, z, vl);                                                             \
        y = __riscv_vfadd_vv_f32m##LMUL(y, x, vl);                                                             \
        y = __riscv_vfadd_vf_f32m##LMUL(y, 1.f, vl);                                                           \
                                                                                                               \
        /* build 2^n */                                                                                        \
        vint32m##LMUL##_t mm = __riscv_vfcvt_x_f_v_i32m##LMUL(fx, vl);                                         \
        mm = __riscv_vadd_vx_i32m##LMUL(mm, 0x7f, vl);                                                         \
        mm = __riscv_vsll_vx_i32m##LMUL(mm, 23, vl);                                                           \
        vfloat32m##LMUL##_t pow2n = __riscv_vreinterpret_v_i32m##LMUL##_f32m##LMUL(mm);                        \
                                                                                                               \
        y = __riscv_vfmul_vv_f32m##LMUL(y, pow2n, vl);                                                         \
        return y;                                                                                              \
    }

_RVV_FLOAT32_EXP_OP(1, 32)
_RVV_FLOAT32_EXP_OP(2, 16)
_RVV_FLOAT32_EXP_OP(4, 8)
_RVV_FLOAT32_EXP_OP(8, 4)

#endif

#endif  // RVV_MATHFUN_FP32_H