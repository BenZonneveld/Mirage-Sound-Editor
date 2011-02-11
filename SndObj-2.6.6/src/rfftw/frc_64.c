/*
 * Copyright (c) 1997-1999 Massachusetts Institute of Technology
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/* This file was automatically generated --- DO NOT EDIT */
/* Generated on Sun Nov  7 20:43:59 EST 1999 */

#include <fftw-int.h>
#include <fftw.h>

/* Generated by: ./genfft -magic-alignment-check -magic-twiddle-load-all -magic-variables 4 -magic-loopi -real2hc 64 */

/*
 * This function contains 394 FP additions, 124 FP multiplications,
 * (or, 342 additions, 72 multiplications, 52 fused multiply/add),
 * 90 stack variables, and 128 memory accesses
 */
static const fftw_real K471396736 = FFTW_KONST(+0.471396736825997648556387625905254377657460319);
static const fftw_real K881921264 = FFTW_KONST(+0.881921264348355029712756863660388349508442621);
static const fftw_real K290284677 = FFTW_KONST(+0.290284677254462367636192375817395274691476278);
static const fftw_real K956940335 = FFTW_KONST(+0.956940335732208864935797886980269969482849206);
static const fftw_real K555570233 = FFTW_KONST(+0.555570233019602224742830813948532874374937191);
static const fftw_real K831469612 = FFTW_KONST(+0.831469612302545237078788377617905756738560812);
static const fftw_real K995184726 = FFTW_KONST(+0.995184726672196886244836953109479921575474869);
static const fftw_real K098017140 = FFTW_KONST(+0.098017140329560601994195563888641845861136673);
static const fftw_real K773010453 = FFTW_KONST(+0.773010453362736960810906609758469800971041293);
static const fftw_real K634393284 = FFTW_KONST(+0.634393284163645498215171613225493370675687095);
static const fftw_real K980785280 = FFTW_KONST(+0.980785280403230449126182236134239036973933731);
static const fftw_real K195090322 = FFTW_KONST(+0.195090322016128267848284868477022240927691618);
static const fftw_real K382683432 = FFTW_KONST(+0.382683432365089771728459984030398866761344562);
static const fftw_real K923879532 = FFTW_KONST(+0.923879532511286756128183189396788286822416626);
static const fftw_real K707106781 = FFTW_KONST(+0.707106781186547524400844362104849039284835938);

/*
 * Generator Id's : 
 * $Id: frc_64.c,v 1.1.1.1 2006/05/12 15:14:44 veplaini Exp $
 * $Id: frc_64.c,v 1.1.1.1 2006/05/12 15:14:44 veplaini Exp $
 * $Id: frc_64.c,v 1.1.1.1 2006/05/12 15:14:44 veplaini Exp $
 */

void fftw_real2hc_64(const fftw_real *input, fftw_real *real_output, fftw_real *imag_output, int istride, int real_ostride, int imag_ostride)
{
     fftw_real tmp269;
     fftw_real tmp7;
     fftw_real tmp14;
     fftw_real tmp15;
     fftw_real tmp320;
     fftw_real tmp67;
     fftw_real tmp209;
     fftw_real tmp179;
     fftw_real tmp238;
     fftw_real tmp301;
     fftw_real tmp361;
     fftw_real tmp381;
     fftw_real tmp312;
     fftw_real tmp143;
     fftw_real tmp227;
     fftw_real tmp167;
     fftw_real tmp230;
     fftw_real tmp158;
     fftw_real tmp231;
     fftw_real tmp162;
     fftw_real tmp228;
     fftw_real tmp308;
     fftw_real tmp313;
     fftw_real tmp364;
     fftw_real tmp382;
     fftw_real tmp22;
     fftw_real tmp29;
     fftw_real tmp30;
     fftw_real tmp272;
     fftw_real tmp321;
     fftw_real tmp74;
     fftw_real tmp239;
     fftw_real tmp176;
     fftw_real tmp210;
     fftw_real tmp284;
     fftw_real tmp354;
     fftw_real tmp378;
     fftw_real tmp295;
     fftw_real tmp121;
     fftw_real tmp223;
     fftw_real tmp127;
     fftw_real tmp220;
     fftw_real tmp110;
     fftw_real tmp221;
     fftw_real tmp130;
     fftw_real tmp224;
     fftw_real tmp291;
     fftw_real tmp296;
     fftw_real tmp357;
     fftw_real tmp379;
     fftw_real tmp89;
     fftw_real tmp213;
     fftw_real tmp92;
     fftw_real tmp212;
     fftw_real tmp46;
     fftw_real tmp374;
     fftw_real tmp276;
     fftw_real tmp317;
     fftw_real tmp80;
     fftw_real tmp215;
     fftw_real tmp83;
     fftw_real tmp216;
     fftw_real tmp61;
     fftw_real tmp375;
     fftw_real tmp279;
     fftw_real tmp318;
     ASSERT_ALIGNED_DOUBLE;
     {
	  fftw_real tmp3;
	  fftw_real tmp63;
	  fftw_real tmp13;
	  fftw_real tmp65;
	  fftw_real tmp6;
	  fftw_real tmp178;
	  fftw_real tmp10;
	  fftw_real tmp64;
	  fftw_real tmp66;
	  fftw_real tmp177;
	  ASSERT_ALIGNED_DOUBLE;
	  {
	       fftw_real tmp1;
	       fftw_real tmp2;
	       fftw_real tmp11;
	       fftw_real tmp12;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp1 = input[0];
	       tmp2 = input[32 * istride];
	       tmp3 = tmp1 + tmp2;
	       tmp63 = tmp1 - tmp2;
	       tmp11 = input[56 * istride];
	       tmp12 = input[24 * istride];
	       tmp13 = tmp11 + tmp12;
	       tmp65 = tmp11 - tmp12;
	  }
	  {
	       fftw_real tmp4;
	       fftw_real tmp5;
	       fftw_real tmp8;
	       fftw_real tmp9;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp4 = input[16 * istride];
	       tmp5 = input[48 * istride];
	       tmp6 = tmp4 + tmp5;
	       tmp178 = tmp4 - tmp5;
	       tmp8 = input[8 * istride];
	       tmp9 = input[40 * istride];
	       tmp10 = tmp8 + tmp9;
	       tmp64 = tmp8 - tmp9;
	  }
	  tmp269 = tmp3 - tmp6;
	  tmp7 = tmp3 + tmp6;
	  tmp14 = tmp10 + tmp13;
	  tmp15 = tmp7 + tmp14;
	  tmp320 = tmp13 - tmp10;
	  tmp66 = K707106781 * (tmp64 + tmp65);
	  tmp67 = tmp63 + tmp66;
	  tmp209 = tmp63 - tmp66;
	  tmp177 = K707106781 * (tmp65 - tmp64);
	  tmp179 = tmp177 - tmp178;
	  tmp238 = tmp178 + tmp177;
     }
     {
	  fftw_real tmp135;
	  fftw_real tmp299;
	  fftw_real tmp166;
	  fftw_real tmp300;
	  fftw_real tmp138;
	  fftw_real tmp311;
	  fftw_real tmp141;
	  fftw_real tmp310;
	  ASSERT_ALIGNED_DOUBLE;
	  {
	       fftw_real tmp133;
	       fftw_real tmp134;
	       fftw_real tmp164;
	       fftw_real tmp165;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp133 = input[63 * istride];
	       tmp134 = input[31 * istride];
	       tmp135 = tmp133 - tmp134;
	       tmp299 = tmp133 + tmp134;
	       tmp164 = input[15 * istride];
	       tmp165 = input[47 * istride];
	       tmp166 = tmp164 - tmp165;
	       tmp300 = tmp164 + tmp165;
	  }
	  {
	       fftw_real tmp136;
	       fftw_real tmp137;
	       fftw_real tmp139;
	       fftw_real tmp140;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp136 = input[7 * istride];
	       tmp137 = input[39 * istride];
	       tmp138 = tmp136 - tmp137;
	       tmp311 = tmp136 + tmp137;
	       tmp139 = input[55 * istride];
	       tmp140 = input[23 * istride];
	       tmp141 = tmp139 - tmp140;
	       tmp310 = tmp139 + tmp140;
	  }
	  {
	       fftw_real tmp359;
	       fftw_real tmp360;
	       fftw_real tmp142;
	       fftw_real tmp163;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp301 = tmp299 - tmp300;
	       tmp359 = tmp299 + tmp300;
	       tmp360 = tmp311 + tmp310;
	       tmp361 = tmp359 + tmp360;
	       tmp381 = tmp359 - tmp360;
	       tmp312 = tmp310 - tmp311;
	       tmp142 = K707106781 * (tmp138 + tmp141);
	       tmp143 = tmp135 + tmp142;
	       tmp227 = tmp135 - tmp142;
	       tmp163 = K707106781 * (tmp141 - tmp138);
	       tmp167 = tmp163 - tmp166;
	       tmp230 = tmp166 + tmp163;
	  }
     }
     {
	  fftw_real tmp146;
	  fftw_real tmp302;
	  fftw_real tmp156;
	  fftw_real tmp306;
	  fftw_real tmp149;
	  fftw_real tmp303;
	  fftw_real tmp153;
	  fftw_real tmp305;
	  ASSERT_ALIGNED_DOUBLE;
	  {
	       fftw_real tmp144;
	       fftw_real tmp145;
	       fftw_real tmp154;
	       fftw_real tmp155;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp144 = input[3 * istride];
	       tmp145 = input[35 * istride];
	       tmp146 = tmp144 - tmp145;
	       tmp302 = tmp144 + tmp145;
	       tmp154 = input[11 * istride];
	       tmp155 = input[43 * istride];
	       tmp156 = tmp154 - tmp155;
	       tmp306 = tmp154 + tmp155;
	  }
	  {
	       fftw_real tmp147;
	       fftw_real tmp148;
	       fftw_real tmp151;
	       fftw_real tmp152;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp147 = input[19 * istride];
	       tmp148 = input[51 * istride];
	       tmp149 = tmp147 - tmp148;
	       tmp303 = tmp147 + tmp148;
	       tmp151 = input[59 * istride];
	       tmp152 = input[27 * istride];
	       tmp153 = tmp151 - tmp152;
	       tmp305 = tmp151 + tmp152;
	  }
	  {
	       fftw_real tmp150;
	       fftw_real tmp157;
	       fftw_real tmp160;
	       fftw_real tmp161;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp150 = (K923879532 * tmp146) - (K382683432 * tmp149);
	       tmp157 = (K923879532 * tmp153) + (K382683432 * tmp156);
	       tmp158 = tmp150 + tmp157;
	       tmp231 = tmp157 - tmp150;
	       tmp160 = (K382683432 * tmp153) - (K923879532 * tmp156);
	       tmp161 = (K382683432 * tmp146) + (K923879532 * tmp149);
	       tmp162 = tmp160 - tmp161;
	       tmp228 = tmp161 + tmp160;
	  }
	  {
	       fftw_real tmp304;
	       fftw_real tmp307;
	       fftw_real tmp362;
	       fftw_real tmp363;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp304 = tmp302 - tmp303;
	       tmp307 = tmp305 - tmp306;
	       tmp308 = K707106781 * (tmp304 + tmp307);
	       tmp313 = K707106781 * (tmp307 - tmp304);
	       tmp362 = tmp302 + tmp303;
	       tmp363 = tmp305 + tmp306;
	       tmp364 = tmp362 + tmp363;
	       tmp382 = tmp363 - tmp362;
	  }
     }
     {
	  fftw_real tmp18;
	  fftw_real tmp68;
	  fftw_real tmp28;
	  fftw_real tmp72;
	  fftw_real tmp21;
	  fftw_real tmp69;
	  fftw_real tmp25;
	  fftw_real tmp71;
	  fftw_real tmp270;
	  fftw_real tmp271;
	  ASSERT_ALIGNED_DOUBLE;
	  {
	       fftw_real tmp16;
	       fftw_real tmp17;
	       fftw_real tmp26;
	       fftw_real tmp27;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp16 = input[4 * istride];
	       tmp17 = input[36 * istride];
	       tmp18 = tmp16 + tmp17;
	       tmp68 = tmp16 - tmp17;
	       tmp26 = input[12 * istride];
	       tmp27 = input[44 * istride];
	       tmp28 = tmp26 + tmp27;
	       tmp72 = tmp26 - tmp27;
	  }
	  {
	       fftw_real tmp19;
	       fftw_real tmp20;
	       fftw_real tmp23;
	       fftw_real tmp24;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp19 = input[20 * istride];
	       tmp20 = input[52 * istride];
	       tmp21 = tmp19 + tmp20;
	       tmp69 = tmp19 - tmp20;
	       tmp23 = input[60 * istride];
	       tmp24 = input[28 * istride];
	       tmp25 = tmp23 + tmp24;
	       tmp71 = tmp23 - tmp24;
	  }
	  tmp22 = tmp18 + tmp21;
	  tmp29 = tmp25 + tmp28;
	  tmp30 = tmp22 + tmp29;
	  tmp270 = tmp18 - tmp21;
	  tmp271 = tmp25 - tmp28;
	  tmp272 = K707106781 * (tmp270 + tmp271);
	  tmp321 = K707106781 * (tmp271 - tmp270);
	  {
	       fftw_real tmp70;
	       fftw_real tmp73;
	       fftw_real tmp174;
	       fftw_real tmp175;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp70 = (K923879532 * tmp68) - (K382683432 * tmp69);
	       tmp73 = (K923879532 * tmp71) + (K382683432 * tmp72);
	       tmp74 = tmp70 + tmp73;
	       tmp239 = tmp73 - tmp70;
	       tmp174 = (K382683432 * tmp71) - (K923879532 * tmp72);
	       tmp175 = (K382683432 * tmp68) + (K923879532 * tmp69);
	       tmp176 = tmp174 - tmp175;
	       tmp210 = tmp175 + tmp174;
	  }
     }
     {
	  fftw_real tmp113;
	  fftw_real tmp293;
	  fftw_real tmp125;
	  fftw_real tmp282;
	  fftw_real tmp116;
	  fftw_real tmp294;
	  fftw_real tmp120;
	  fftw_real tmp283;
	  ASSERT_ALIGNED_DOUBLE;
	  {
	       fftw_real tmp111;
	       fftw_real tmp112;
	       fftw_real tmp123;
	       fftw_real tmp124;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp111 = input[57 * istride];
	       tmp112 = input[25 * istride];
	       tmp113 = tmp111 - tmp112;
	       tmp293 = tmp111 + tmp112;
	       tmp123 = input[istride];
	       tmp124 = input[33 * istride];
	       tmp125 = tmp123 - tmp124;
	       tmp282 = tmp123 + tmp124;
	  }
	  {
	       fftw_real tmp114;
	       fftw_real tmp115;
	       fftw_real tmp118;
	       fftw_real tmp119;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp114 = input[9 * istride];
	       tmp115 = input[41 * istride];
	       tmp116 = tmp114 - tmp115;
	       tmp294 = tmp114 + tmp115;
	       tmp118 = input[17 * istride];
	       tmp119 = input[49 * istride];
	       tmp120 = tmp118 - tmp119;
	       tmp283 = tmp118 + tmp119;
	  }
	  {
	       fftw_real tmp352;
	       fftw_real tmp353;
	       fftw_real tmp117;
	       fftw_real tmp126;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp284 = tmp282 - tmp283;
	       tmp352 = tmp282 + tmp283;
	       tmp353 = tmp294 + tmp293;
	       tmp354 = tmp352 + tmp353;
	       tmp378 = tmp352 - tmp353;
	       tmp295 = tmp293 - tmp294;
	       tmp117 = K707106781 * (tmp113 - tmp116);
	       tmp121 = tmp117 - tmp120;
	       tmp223 = tmp120 + tmp117;
	       tmp126 = K707106781 * (tmp116 + tmp113);
	       tmp127 = tmp125 + tmp126;
	       tmp220 = tmp125 - tmp126;
	  }
     }
     {
	  fftw_real tmp98;
	  fftw_real tmp288;
	  fftw_real tmp108;
	  fftw_real tmp286;
	  fftw_real tmp101;
	  fftw_real tmp289;
	  fftw_real tmp105;
	  fftw_real tmp285;
	  ASSERT_ALIGNED_DOUBLE;
	  {
	       fftw_real tmp96;
	       fftw_real tmp97;
	       fftw_real tmp106;
	       fftw_real tmp107;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp96 = input[61 * istride];
	       tmp97 = input[29 * istride];
	       tmp98 = tmp96 - tmp97;
	       tmp288 = tmp96 + tmp97;
	       tmp106 = input[21 * istride];
	       tmp107 = input[53 * istride];
	       tmp108 = tmp106 - tmp107;
	       tmp286 = tmp106 + tmp107;
	  }
	  {
	       fftw_real tmp99;
	       fftw_real tmp100;
	       fftw_real tmp103;
	       fftw_real tmp104;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp99 = input[13 * istride];
	       tmp100 = input[45 * istride];
	       tmp101 = tmp99 - tmp100;
	       tmp289 = tmp99 + tmp100;
	       tmp103 = input[5 * istride];
	       tmp104 = input[37 * istride];
	       tmp105 = tmp103 - tmp104;
	       tmp285 = tmp103 + tmp104;
	  }
	  {
	       fftw_real tmp102;
	       fftw_real tmp109;
	       fftw_real tmp128;
	       fftw_real tmp129;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp102 = (K382683432 * tmp98) - (K923879532 * tmp101);
	       tmp109 = (K382683432 * tmp105) + (K923879532 * tmp108);
	       tmp110 = tmp102 - tmp109;
	       tmp221 = tmp109 + tmp102;
	       tmp128 = (K923879532 * tmp105) - (K382683432 * tmp108);
	       tmp129 = (K923879532 * tmp98) + (K382683432 * tmp101);
	       tmp130 = tmp128 + tmp129;
	       tmp224 = tmp129 - tmp128;
	  }
	  {
	       fftw_real tmp287;
	       fftw_real tmp290;
	       fftw_real tmp355;
	       fftw_real tmp356;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp287 = tmp285 - tmp286;
	       tmp290 = tmp288 - tmp289;
	       tmp291 = K707106781 * (tmp287 + tmp290);
	       tmp296 = K707106781 * (tmp290 - tmp287);
	       tmp355 = tmp285 + tmp286;
	       tmp356 = tmp288 + tmp289;
	       tmp357 = tmp355 + tmp356;
	       tmp379 = tmp356 - tmp355;
	  }
     }
     {
	  fftw_real tmp34;
	  fftw_real tmp90;
	  fftw_real tmp44;
	  fftw_real tmp85;
	  fftw_real tmp37;
	  fftw_real tmp88;
	  fftw_real tmp41;
	  fftw_real tmp86;
	  fftw_real tmp87;
	  fftw_real tmp91;
	  ASSERT_ALIGNED_DOUBLE;
	  {
	       fftw_real tmp32;
	       fftw_real tmp33;
	       fftw_real tmp42;
	       fftw_real tmp43;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp32 = input[2 * istride];
	       tmp33 = input[34 * istride];
	       tmp34 = tmp32 + tmp33;
	       tmp90 = tmp32 - tmp33;
	       tmp42 = input[58 * istride];
	       tmp43 = input[26 * istride];
	       tmp44 = tmp42 + tmp43;
	       tmp85 = tmp42 - tmp43;
	  }
	  {
	       fftw_real tmp35;
	       fftw_real tmp36;
	       fftw_real tmp39;
	       fftw_real tmp40;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp35 = input[18 * istride];
	       tmp36 = input[50 * istride];
	       tmp37 = tmp35 + tmp36;
	       tmp88 = tmp35 - tmp36;
	       tmp39 = input[10 * istride];
	       tmp40 = input[42 * istride];
	       tmp41 = tmp39 + tmp40;
	       tmp86 = tmp39 - tmp40;
	  }
	  tmp87 = K707106781 * (tmp85 - tmp86);
	  tmp89 = tmp87 - tmp88;
	  tmp213 = tmp88 + tmp87;
	  tmp91 = K707106781 * (tmp86 + tmp85);
	  tmp92 = tmp90 + tmp91;
	  tmp212 = tmp90 - tmp91;
	  {
	       fftw_real tmp38;
	       fftw_real tmp45;
	       fftw_real tmp274;
	       fftw_real tmp275;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp38 = tmp34 + tmp37;
	       tmp45 = tmp41 + tmp44;
	       tmp46 = tmp38 + tmp45;
	       tmp374 = tmp38 - tmp45;
	       tmp274 = tmp34 - tmp37;
	       tmp275 = tmp44 - tmp41;
	       tmp276 = (K923879532 * tmp274) + (K382683432 * tmp275);
	       tmp317 = (K923879532 * tmp275) - (K382683432 * tmp274);
	  }
     }
     {
	  fftw_real tmp49;
	  fftw_real tmp76;
	  fftw_real tmp59;
	  fftw_real tmp78;
	  fftw_real tmp52;
	  fftw_real tmp82;
	  fftw_real tmp56;
	  fftw_real tmp77;
	  fftw_real tmp79;
	  fftw_real tmp81;
	  ASSERT_ALIGNED_DOUBLE;
	  {
	       fftw_real tmp47;
	       fftw_real tmp48;
	       fftw_real tmp57;
	       fftw_real tmp58;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp47 = input[62 * istride];
	       tmp48 = input[30 * istride];
	       tmp49 = tmp47 + tmp48;
	       tmp76 = tmp47 - tmp48;
	       tmp57 = input[54 * istride];
	       tmp58 = input[22 * istride];
	       tmp59 = tmp57 + tmp58;
	       tmp78 = tmp57 - tmp58;
	  }
	  {
	       fftw_real tmp50;
	       fftw_real tmp51;
	       fftw_real tmp54;
	       fftw_real tmp55;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp50 = input[14 * istride];
	       tmp51 = input[46 * istride];
	       tmp52 = tmp50 + tmp51;
	       tmp82 = tmp50 - tmp51;
	       tmp54 = input[6 * istride];
	       tmp55 = input[38 * istride];
	       tmp56 = tmp54 + tmp55;
	       tmp77 = tmp54 - tmp55;
	  }
	  tmp79 = K707106781 * (tmp77 + tmp78);
	  tmp80 = tmp76 + tmp79;
	  tmp215 = tmp76 - tmp79;
	  tmp81 = K707106781 * (tmp78 - tmp77);
	  tmp83 = tmp81 - tmp82;
	  tmp216 = tmp82 + tmp81;
	  {
	       fftw_real tmp53;
	       fftw_real tmp60;
	       fftw_real tmp277;
	       fftw_real tmp278;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp53 = tmp49 + tmp52;
	       tmp60 = tmp56 + tmp59;
	       tmp61 = tmp53 + tmp60;
	       tmp375 = tmp53 - tmp60;
	       tmp277 = tmp49 - tmp52;
	       tmp278 = tmp59 - tmp56;
	       tmp279 = (K923879532 * tmp277) - (K382683432 * tmp278);
	       tmp318 = (K382683432 * tmp277) + (K923879532 * tmp278);
	  }
     }
     {
	  fftw_real tmp31;
	  fftw_real tmp62;
	  fftw_real tmp369;
	  fftw_real tmp370;
	  fftw_real tmp371;
	  fftw_real tmp372;
	  ASSERT_ALIGNED_DOUBLE;
	  tmp31 = tmp15 + tmp30;
	  tmp62 = tmp46 + tmp61;
	  tmp369 = tmp31 + tmp62;
	  tmp370 = tmp354 + tmp357;
	  tmp371 = tmp361 + tmp364;
	  tmp372 = tmp370 + tmp371;
	  real_output[16 * real_ostride] = tmp31 - tmp62;
	  imag_output[16 * imag_ostride] = tmp371 - tmp370;
	  real_output[32 * real_ostride] = tmp369 - tmp372;
	  real_output[0] = tmp369 + tmp372;
     }
     {
	  fftw_real tmp351;
	  fftw_real tmp367;
	  fftw_real tmp366;
	  fftw_real tmp368;
	  fftw_real tmp358;
	  fftw_real tmp365;
	  ASSERT_ALIGNED_DOUBLE;
	  tmp351 = tmp15 - tmp30;
	  tmp367 = tmp61 - tmp46;
	  tmp358 = tmp354 - tmp357;
	  tmp365 = tmp361 - tmp364;
	  tmp366 = K707106781 * (tmp358 + tmp365);
	  tmp368 = K707106781 * (tmp365 - tmp358);
	  real_output[24 * real_ostride] = tmp351 - tmp366;
	  real_output[8 * real_ostride] = tmp351 + tmp366;
	  imag_output[8 * imag_ostride] = tmp367 + tmp368;
	  imag_output[24 * imag_ostride] = tmp368 - tmp367;
     }
     {
	  fftw_real tmp75;
	  fftw_real tmp189;
	  fftw_real tmp94;
	  fftw_real tmp199;
	  fftw_real tmp173;
	  fftw_real tmp190;
	  fftw_real tmp132;
	  fftw_real tmp184;
	  fftw_real tmp180;
	  fftw_real tmp200;
	  fftw_real tmp194;
	  fftw_real tmp204;
	  fftw_real tmp169;
	  fftw_real tmp185;
	  fftw_real tmp197;
	  fftw_real tmp205;
	  ASSERT_ALIGNED_DOUBLE;
	  {
	       fftw_real tmp84;
	       fftw_real tmp93;
	       fftw_real tmp192;
	       fftw_real tmp193;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp75 = tmp67 - tmp74;
	       tmp189 = tmp67 + tmp74;
	       tmp84 = (K195090322 * tmp80) + (K980785280 * tmp83);
	       tmp93 = (K980785280 * tmp89) - (K195090322 * tmp92);
	       tmp94 = tmp84 - tmp93;
	       tmp199 = tmp93 + tmp84;
	       {
		    fftw_real tmp171;
		    fftw_real tmp172;
		    fftw_real tmp122;
		    fftw_real tmp131;
		    ASSERT_ALIGNED_DOUBLE;
		    tmp171 = (K980785280 * tmp80) - (K195090322 * tmp83);
		    tmp172 = (K980785280 * tmp92) + (K195090322 * tmp89);
		    tmp173 = tmp171 - tmp172;
		    tmp190 = tmp172 + tmp171;
		    tmp122 = tmp110 - tmp121;
		    tmp131 = tmp127 - tmp130;
		    tmp132 = (K634393284 * tmp122) + (K773010453 * tmp131);
		    tmp184 = (K773010453 * tmp122) - (K634393284 * tmp131);
	       }
	       tmp180 = tmp176 - tmp179;
	       tmp200 = tmp179 + tmp176;
	       tmp192 = tmp121 + tmp110;
	       tmp193 = tmp127 + tmp130;
	       tmp194 = (K098017140 * tmp192) + (K995184726 * tmp193);
	       tmp204 = (K995184726 * tmp192) - (K098017140 * tmp193);
	       {
		    fftw_real tmp159;
		    fftw_real tmp168;
		    fftw_real tmp195;
		    fftw_real tmp196;
		    ASSERT_ALIGNED_DOUBLE;
		    tmp159 = tmp143 - tmp158;
		    tmp168 = tmp162 - tmp167;
		    tmp169 = (K773010453 * tmp159) - (K634393284 * tmp168);
		    tmp185 = (K773010453 * tmp168) + (K634393284 * tmp159);
		    tmp195 = tmp143 + tmp158;
		    tmp196 = tmp167 + tmp162;
		    tmp197 = (K995184726 * tmp195) - (K098017140 * tmp196);
		    tmp205 = (K995184726 * tmp196) + (K098017140 * tmp195);
	       }
	  }
	  {
	       fftw_real tmp95;
	       fftw_real tmp170;
	       fftw_real tmp181;
	       fftw_real tmp182;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp95 = tmp75 + tmp94;
	       tmp170 = tmp132 + tmp169;
	       real_output[25 * real_ostride] = tmp95 - tmp170;
	       real_output[7 * real_ostride] = tmp95 + tmp170;
	       tmp181 = tmp173 - tmp180;
	       tmp182 = tmp169 - tmp132;
	       imag_output[9 * imag_ostride] = tmp181 + tmp182;
	       imag_output[23 * imag_ostride] = tmp182 - tmp181;
	  }
	  {
	       fftw_real tmp183;
	       fftw_real tmp186;
	       fftw_real tmp187;
	       fftw_real tmp188;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp183 = tmp180 + tmp173;
	       tmp186 = tmp184 + tmp185;
	       imag_output[7 * imag_ostride] = tmp183 + tmp186;
	       imag_output[25 * imag_ostride] = tmp186 - tmp183;
	       tmp187 = tmp75 - tmp94;
	       tmp188 = tmp185 - tmp184;
	       real_output[23 * real_ostride] = tmp187 - tmp188;
	       real_output[9 * real_ostride] = tmp187 + tmp188;
	  }
	  {
	       fftw_real tmp191;
	       fftw_real tmp198;
	       fftw_real tmp201;
	       fftw_real tmp202;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp191 = tmp189 + tmp190;
	       tmp198 = tmp194 + tmp197;
	       real_output[31 * real_ostride] = tmp191 - tmp198;
	       real_output[real_ostride] = tmp191 + tmp198;
	       tmp201 = tmp199 - tmp200;
	       tmp202 = tmp197 - tmp194;
	       imag_output[15 * imag_ostride] = tmp201 + tmp202;
	       imag_output[17 * imag_ostride] = tmp202 - tmp201;
	  }
	  {
	       fftw_real tmp203;
	       fftw_real tmp206;
	       fftw_real tmp207;
	       fftw_real tmp208;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp203 = tmp200 + tmp199;
	       tmp206 = tmp204 + tmp205;
	       imag_output[imag_ostride] = tmp203 + tmp206;
	       imag_output[31 * imag_ostride] = tmp206 - tmp203;
	       tmp207 = tmp189 - tmp190;
	       tmp208 = tmp205 - tmp204;
	       real_output[17 * real_ostride] = tmp207 - tmp208;
	       real_output[15 * real_ostride] = tmp207 + tmp208;
	  }
     }
     {
	  fftw_real tmp377;
	  fftw_real tmp393;
	  fftw_real tmp387;
	  fftw_real tmp389;
	  fftw_real tmp384;
	  fftw_real tmp388;
	  fftw_real tmp392;
	  fftw_real tmp394;
	  ASSERT_ALIGNED_DOUBLE;
	  {
	       fftw_real tmp373;
	       fftw_real tmp376;
	       fftw_real tmp385;
	       fftw_real tmp386;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp373 = tmp7 - tmp14;
	       tmp376 = K707106781 * (tmp374 + tmp375);
	       tmp377 = tmp373 + tmp376;
	       tmp393 = tmp373 - tmp376;
	       tmp385 = K707106781 * (tmp375 - tmp374);
	       tmp386 = tmp29 - tmp22;
	       tmp387 = tmp385 - tmp386;
	       tmp389 = tmp386 + tmp385;
	  }
	  {
	       fftw_real tmp380;
	       fftw_real tmp383;
	       fftw_real tmp390;
	       fftw_real tmp391;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp380 = (K923879532 * tmp378) + (K382683432 * tmp379);
	       tmp383 = (K923879532 * tmp381) - (K382683432 * tmp382);
	       tmp384 = tmp380 + tmp383;
	       tmp388 = tmp383 - tmp380;
	       tmp390 = (K923879532 * tmp379) - (K382683432 * tmp378);
	       tmp391 = (K382683432 * tmp381) + (K923879532 * tmp382);
	       tmp392 = tmp390 + tmp391;
	       tmp394 = tmp391 - tmp390;
	  }
	  real_output[28 * real_ostride] = tmp377 - tmp384;
	  real_output[4 * real_ostride] = tmp377 + tmp384;
	  imag_output[12 * imag_ostride] = tmp387 + tmp388;
	  imag_output[20 * imag_ostride] = tmp388 - tmp387;
	  imag_output[4 * imag_ostride] = tmp389 + tmp392;
	  imag_output[28 * imag_ostride] = tmp392 - tmp389;
	  real_output[20 * real_ostride] = tmp393 - tmp394;
	  real_output[12 * real_ostride] = tmp393 + tmp394;
     }
     {
	  fftw_real tmp281;
	  fftw_real tmp329;
	  fftw_real tmp323;
	  fftw_real tmp325;
	  fftw_real tmp298;
	  fftw_real tmp326;
	  fftw_real tmp315;
	  fftw_real tmp327;
	  ASSERT_ALIGNED_DOUBLE;
	  {
	       fftw_real tmp273;
	       fftw_real tmp280;
	       fftw_real tmp319;
	       fftw_real tmp322;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp273 = tmp269 + tmp272;
	       tmp280 = tmp276 + tmp279;
	       tmp281 = tmp273 + tmp280;
	       tmp329 = tmp273 - tmp280;
	       tmp319 = tmp317 + tmp318;
	       tmp322 = tmp320 + tmp321;
	       tmp323 = tmp319 - tmp322;
	       tmp325 = tmp322 + tmp319;
	  }
	  {
	       fftw_real tmp292;
	       fftw_real tmp297;
	       fftw_real tmp309;
	       fftw_real tmp314;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp292 = tmp284 + tmp291;
	       tmp297 = tmp295 + tmp296;
	       tmp298 = (K980785280 * tmp292) + (K195090322 * tmp297);
	       tmp326 = (K980785280 * tmp297) - (K195090322 * tmp292);
	       tmp309 = tmp301 + tmp308;
	       tmp314 = tmp312 + tmp313;
	       tmp315 = (K980785280 * tmp309) - (K195090322 * tmp314);
	       tmp327 = (K195090322 * tmp309) + (K980785280 * tmp314);
	  }
	  {
	       fftw_real tmp316;
	       fftw_real tmp324;
	       fftw_real tmp328;
	       fftw_real tmp330;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp316 = tmp298 + tmp315;
	       real_output[30 * real_ostride] = tmp281 - tmp316;
	       real_output[2 * real_ostride] = tmp281 + tmp316;
	       tmp324 = tmp315 - tmp298;
	       imag_output[14 * imag_ostride] = tmp323 + tmp324;
	       imag_output[18 * imag_ostride] = tmp324 - tmp323;
	       tmp328 = tmp326 + tmp327;
	       imag_output[2 * imag_ostride] = tmp325 + tmp328;
	       imag_output[30 * imag_ostride] = tmp328 - tmp325;
	       tmp330 = tmp327 - tmp326;
	       real_output[18 * real_ostride] = tmp329 - tmp330;
	       real_output[14 * real_ostride] = tmp329 + tmp330;
	  }
     }
     {
	  fftw_real tmp333;
	  fftw_real tmp349;
	  fftw_real tmp343;
	  fftw_real tmp345;
	  fftw_real tmp336;
	  fftw_real tmp346;
	  fftw_real tmp339;
	  fftw_real tmp347;
	  ASSERT_ALIGNED_DOUBLE;
	  {
	       fftw_real tmp331;
	       fftw_real tmp332;
	       fftw_real tmp341;
	       fftw_real tmp342;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp331 = tmp269 - tmp272;
	       tmp332 = tmp318 - tmp317;
	       tmp333 = tmp331 + tmp332;
	       tmp349 = tmp331 - tmp332;
	       tmp341 = tmp279 - tmp276;
	       tmp342 = tmp321 - tmp320;
	       tmp343 = tmp341 - tmp342;
	       tmp345 = tmp342 + tmp341;
	  }
	  {
	       fftw_real tmp334;
	       fftw_real tmp335;
	       fftw_real tmp337;
	       fftw_real tmp338;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp334 = tmp284 - tmp291;
	       tmp335 = tmp296 - tmp295;
	       tmp336 = (K831469612 * tmp334) + (K555570233 * tmp335);
	       tmp346 = (K831469612 * tmp335) - (K555570233 * tmp334);
	       tmp337 = tmp301 - tmp308;
	       tmp338 = tmp313 - tmp312;
	       tmp339 = (K831469612 * tmp337) - (K555570233 * tmp338);
	       tmp347 = (K555570233 * tmp337) + (K831469612 * tmp338);
	  }
	  {
	       fftw_real tmp340;
	       fftw_real tmp344;
	       fftw_real tmp348;
	       fftw_real tmp350;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp340 = tmp336 + tmp339;
	       real_output[26 * real_ostride] = tmp333 - tmp340;
	       real_output[6 * real_ostride] = tmp333 + tmp340;
	       tmp344 = tmp339 - tmp336;
	       imag_output[10 * imag_ostride] = tmp343 + tmp344;
	       imag_output[22 * imag_ostride] = tmp344 - tmp343;
	       tmp348 = tmp346 + tmp347;
	       imag_output[6 * imag_ostride] = tmp345 + tmp348;
	       imag_output[26 * imag_ostride] = tmp348 - tmp345;
	       tmp350 = tmp347 - tmp346;
	       real_output[22 * real_ostride] = tmp349 - tmp350;
	       real_output[10 * real_ostride] = tmp349 + tmp350;
	  }
     }
     {
	  fftw_real tmp211;
	  fftw_real tmp249;
	  fftw_real tmp218;
	  fftw_real tmp259;
	  fftw_real tmp237;
	  fftw_real tmp250;
	  fftw_real tmp226;
	  fftw_real tmp244;
	  fftw_real tmp240;
	  fftw_real tmp260;
	  fftw_real tmp254;
	  fftw_real tmp264;
	  fftw_real tmp233;
	  fftw_real tmp245;
	  fftw_real tmp257;
	  fftw_real tmp265;
	  ASSERT_ALIGNED_DOUBLE;
	  {
	       fftw_real tmp214;
	       fftw_real tmp217;
	       fftw_real tmp252;
	       fftw_real tmp253;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp211 = tmp209 + tmp210;
	       tmp249 = tmp209 - tmp210;
	       tmp214 = (K831469612 * tmp212) + (K555570233 * tmp213);
	       tmp217 = (K831469612 * tmp215) - (K555570233 * tmp216);
	       tmp218 = tmp214 + tmp217;
	       tmp259 = tmp217 - tmp214;
	       {
		    fftw_real tmp235;
		    fftw_real tmp236;
		    fftw_real tmp222;
		    fftw_real tmp225;
		    ASSERT_ALIGNED_DOUBLE;
		    tmp235 = (K831469612 * tmp213) - (K555570233 * tmp212);
		    tmp236 = (K555570233 * tmp215) + (K831469612 * tmp216);
		    tmp237 = tmp235 + tmp236;
		    tmp250 = tmp236 - tmp235;
		    tmp222 = tmp220 + tmp221;
		    tmp225 = tmp223 + tmp224;
		    tmp226 = (K956940335 * tmp222) + (K290284677 * tmp225);
		    tmp244 = (K956940335 * tmp225) - (K290284677 * tmp222);
	       }
	       tmp240 = tmp238 + tmp239;
	       tmp260 = tmp239 - tmp238;
	       tmp252 = tmp220 - tmp221;
	       tmp253 = tmp224 - tmp223;
	       tmp254 = (K881921264 * tmp252) + (K471396736 * tmp253);
	       tmp264 = (K881921264 * tmp253) - (K471396736 * tmp252);
	       {
		    fftw_real tmp229;
		    fftw_real tmp232;
		    fftw_real tmp255;
		    fftw_real tmp256;
		    ASSERT_ALIGNED_DOUBLE;
		    tmp229 = tmp227 + tmp228;
		    tmp232 = tmp230 + tmp231;
		    tmp233 = (K956940335 * tmp229) - (K290284677 * tmp232);
		    tmp245 = (K290284677 * tmp229) + (K956940335 * tmp232);
		    tmp255 = tmp227 - tmp228;
		    tmp256 = tmp231 - tmp230;
		    tmp257 = (K881921264 * tmp255) - (K471396736 * tmp256);
		    tmp265 = (K471396736 * tmp255) + (K881921264 * tmp256);
	       }
	  }
	  {
	       fftw_real tmp219;
	       fftw_real tmp234;
	       fftw_real tmp241;
	       fftw_real tmp242;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp219 = tmp211 + tmp218;
	       tmp234 = tmp226 + tmp233;
	       real_output[29 * real_ostride] = tmp219 - tmp234;
	       real_output[3 * real_ostride] = tmp219 + tmp234;
	       tmp241 = tmp237 - tmp240;
	       tmp242 = tmp233 - tmp226;
	       imag_output[13 * imag_ostride] = tmp241 + tmp242;
	       imag_output[19 * imag_ostride] = tmp242 - tmp241;
	  }
	  {
	       fftw_real tmp243;
	       fftw_real tmp246;
	       fftw_real tmp247;
	       fftw_real tmp248;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp243 = tmp240 + tmp237;
	       tmp246 = tmp244 + tmp245;
	       imag_output[3 * imag_ostride] = tmp243 + tmp246;
	       imag_output[29 * imag_ostride] = tmp246 - tmp243;
	       tmp247 = tmp211 - tmp218;
	       tmp248 = tmp245 - tmp244;
	       real_output[19 * real_ostride] = tmp247 - tmp248;
	       real_output[13 * real_ostride] = tmp247 + tmp248;
	  }
	  {
	       fftw_real tmp251;
	       fftw_real tmp258;
	       fftw_real tmp261;
	       fftw_real tmp262;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp251 = tmp249 + tmp250;
	       tmp258 = tmp254 + tmp257;
	       real_output[27 * real_ostride] = tmp251 - tmp258;
	       real_output[5 * real_ostride] = tmp251 + tmp258;
	       tmp261 = tmp259 - tmp260;
	       tmp262 = tmp257 - tmp254;
	       imag_output[11 * imag_ostride] = tmp261 + tmp262;
	       imag_output[21 * imag_ostride] = tmp262 - tmp261;
	  }
	  {
	       fftw_real tmp263;
	       fftw_real tmp266;
	       fftw_real tmp267;
	       fftw_real tmp268;
	       ASSERT_ALIGNED_DOUBLE;
	       tmp263 = tmp260 + tmp259;
	       tmp266 = tmp264 + tmp265;
	       imag_output[5 * imag_ostride] = tmp263 + tmp266;
	       imag_output[27 * imag_ostride] = tmp266 - tmp263;
	       tmp267 = tmp249 - tmp250;
	       tmp268 = tmp265 - tmp264;
	       real_output[21 * real_ostride] = tmp267 - tmp268;
	       real_output[11 * real_ostride] = tmp267 + tmp268;
	  }
     }
}

fftw_codelet_desc fftw_real2hc_64_desc =
{
     "fftw_real2hc_64",
     (void (*)()) fftw_real2hc_64,
     64,
     FFTW_FORWARD,
     FFTW_REAL2HC,
     1410,
     0,
     (const int *) 0,
};
