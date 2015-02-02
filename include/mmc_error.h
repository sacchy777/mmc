/*
mmc_error.h - A yet another mml to midi converter

Copyright (c) 2015 sada.gussy at gmail dot com

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

   3. This notice may not be removed or altered from any source
   distribution.
*/
#ifndef MMC_ERROR_H_
#define MMC_ERROR_H_

#define MMC_MSG_ERROR_CONVERT_FAILED "[MMC ERROR] Convert failed due to errors"
#define MMC_MSG_ERROR_OUTOFMEMORY "[MMC ERROR] out of memory at %s"
#define MMC_MSG_ERROR_UNSUPPORTED_TOKEN "[MMC ERROR(%d:%d)] unsupported token (%c)"
#define MMC_MSG_ERROR_OUTOFRANGE "[MMC ERROR(%d:%d)] %s %d out of range"
#define MMC_MSG_WARNING_OUTOFRANGE "[MMC WARNING(%d:%d)] %s %d out of range, set to %d"
#define MMC_MSG_WARNING_SIGN_IGNORED "[MMC WARNING(%d:%d)] %s sign ignored"
#define MMC_MSG_ERROR_PARAM_MISSING "[MMC ERROR(%d:%d)] Parameter %s missing"
#define MMC_MSG_ERROR_SYNTAX "[MMC ERROR(%d:%d)] syntax error at %s"
#define MMC_MSG_ERROR_UNEXPECTED_TOKEN "[MMC ERROR(%d:%d)] unexpected token (%s)"
#define MMC_MSG_ERROR_BRACKET_NEST_MAX "[MMC ERROR(%d:%d)] bracket nest max reached."
#define MMC_MSG_ERROR_BRACKET_REPEAT_ILLEGAL "[MMC ERROR(%d:%d)] bracket repeat number illegal."
#define MMC_MSG_ERROR_BRACKET_NUMBER_MISMATCH "[MMC ERROR] bracket number mismatch."

#endif

