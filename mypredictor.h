/*Copyright (c) <2018>, INRIA : Institut National de Recherche en Informatique et en Automatique (French National Research Institute for Computer Science and Applied Mathematics)
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.*/

/* Updated version of the same predictor for the Value prediction Championship
 * held in CS6354 at the University of Virginia
 *
 * @author khyati@virginia.edu
 * @author alenkruth@virginia.edu
 *
 * */
/* Same predictor for the 3 tracks, but with different parameters*/

#include <vector>
#include <deque>
#define PREDSTRIDE
#define PREDVTAGE

// 32KB //
// #define K32
#ifdef K32

// 4.202 //3.729 for stride only  //3.570 for VTAGE only 
// 262018 bits
#define UWIDTH 2
#define LOGLDATA 9
#define LOGBANK 7
#define TAGWIDTH 11
#define NBBANK 49


#define NHIST 8
int HL[NHIST + 1] = { 0, 0, 3, 7, 15, 31, 63, 90, 127 };

#define LOGSTR 4
#define NBWAYSTR 3
#define TAGWIDTHSTR 14
#define LOGSTRIDE 20
#endif
//END 32 KB//

// custom CS6354 16K//
#define CS6354K16
#ifdef CS6354K16

// ?????? //?????? for stride only  //????? for VTAGE only 
// ???????? bits
#define UWIDTH 1 // K32 - 2 | K8 - 2 | KUL - 1 
#define LOGLDATA 8 // K32 - 9 | K8 - 7 | KUL - 20
#define LOGBANK 6 // K32 - 7 | K8 - 5| KUL - 20
#define TAGWIDTH 14 // K32 - 11 | K8 - 11| KUL - 15
#define NBBANK 48 // K32 - 49 | K8 - 47| KUL - 63

#define NHIST 13 // K32 - 8 | K8 - 7 | KUL - 14
int HL[NHIST + 1] = { 0, 0, 1, 3, 6, 12, 18, 30, 63, 90, 127, 127, 127, 127};
// K32 - int HL[NHIST + 1] = {0, 0, 3, 7, 15, 31, 63, 90, 127}
// K8 - int HL[NHIST + 1] = {0, 0, 1, 3, 6, 12, 18, 30}
// KUL - int HL[NHIST + 1] = {0, 0, 1, 3, 7, 15, 31, 47, 63, 95, 127, 191, 255, 383, 511}

#define LOGSTR 2 // K32 - 4 | K8 - 4 | KUL - 20 
#define NBWAYSTR 1 // K32 - 3 | K8 - 3 | KUL - 15
#define TAGWIDTHSTR 5 // K32 - 14 | K8 - 14 | KUL - 30
#define LOGSTRIDE 2 // K32 - 20 | K8 - 20 | KUL - 3
#endif
//END custom CS6354 16K//

// 8KB //
// #define K8
#ifdef K8
// 8KB
// 4.026 //3.729 Stride only // 3.437 for TAGE  only
// 65378 bits
#define UWIDTH 2
#define LOGLDATA 7
#define LOGBANK 5
#define TAGWIDTH 11
#define NBBANK 47

#define NHIST 7
int HL[NHIST + 1] = { 0, 0, 1, 3, 6, 12, 18, 30 };

#define LOGSTR 4
#define NBWAYSTR 3
#define TAGWIDTHSTR 14
#define LOGSTRIDE 20
#endif
//END 8KB //


//UNLIMITED//
//#define LIMITSTUDY
#ifdef LIMITSTUDY
// 4.408 //3.730 Stride only // 3.732 for TAGE  only
#define UWIDTH 1
#define LOGLDATA 20
#define LOGBANK 20
#define TAGWIDTH 15
#define NBBANK 63



#define NHIST 14
int HL[NHIST + 1] =
  { 0, 0, 1, 3, 7, 15, 31, 47, 63, 95, 127, 191, 255, 383, 511 };
#define LOGSTR 20
#define TAGWIDTHSTR 15
#define LOGSTRIDE 30
#define NBWAYSTR 3

#endif
//END UNLIMITED //

#define WIDTHCONFID 3
#define MAXCONFID ((1<< WIDTHCONFID)-1)
#define WIDTHCONFIDSTR 5
#define MAXCONFIDSTR  ((1<< WIDTHCONFIDSTR)-1)
#define MAXU  ((1<< UWIDTH)-1)

#define BANKDATA (1<<LOGLDATA)
#define MINSTRIDE -(1<<(LOGSTRIDE-1))
#define MAXSTRIDE (-MINSTRIDE-1)
#define BANKSIZE (1<<LOGBANK)
#define PREDSIZE (NBBANK*BANKSIZE)

#include <unordered_map>

std::unordered_map<uint64_t, uint8_t> memory;


// Global path history

static uint64_t gpath[8] = {0, 0, 0, 0, 0, 0, 0, 0};

/* using up to 512 bits of path history was found to result in some performance benefit : essentially in the unlimited case. I did not explore longer histories */

static uint64_t gtargeth = 0;
/* history of the targets : limited to  64 bits*/


// The E-Stride predictor
//entry in the stride predictor
struct strdata
{
  uint64_t LastValue;		//64 bits
  uint64_t Stride;		// LOGSTRIDE bits
  uint8_t conf;			// WIDTHCONFIDSTR bits
  uint16_t tag;			//TAGWIDTHSTR bits
  uint16_t NotFirstOcc;		//1 bits
  int u;			// 2 bits
  //67 + LOGSTRIDE + WIDTHCONFIDSTR + TAGWIDTHSTR bits 
};
static strdata STR[NBWAYSTR * (1 << LOGSTR)];


static int SafeStride = 0;	// 16 bits

/////////////////////////////////// For E-VTAGE
//the data values
struct longdata
{
  uint64_t data;
  uint8_t u;
};
static longdata LDATA[3 * BANKDATA];
//  managed as a a skewed associative array
//each entry is 64-LOGLDATA bits for the data (since the other bits can be deduced from the index) + 2 bits for u

//VTAGE
struct vtentry
{
  uint64_t hashpt;		// hash of the value + number of way in the value array ; LOGLDATA + 2 bits 
  uint8_t conf;			//WIDTHCONFID bits
  uint16_t tag;			// TAGWIDTH bits
  uint8_t u;			//2 bits
  //LOGLDATA +4 +WIDTHCONFID +TAGWIDTH bits
};

static vtentry Vtage[PREDSIZE];

#define  MAXTICK 256
static int TICK;		//10 bits // for managing replacement on the VTAGE entries
static int LastMispVT = 0;	//8 bits //for tracking the last misprediction on VTAGE

uint64_t stat_correct_mem_tracked = 0;
uint64_t stat_incorrect_mem_tracked_init = 0;
uint64_t stat_incorrect_mem_tracked_alias = 0;
uint64_t stat_incorrect_mem_tracked_ld_no_output = 0;

 //index function for VTAGE (use the global path history): just a complex hash function
uint32_t
gi (int i, uint64_t pc)
{
  int hl = (HL[i] < 64) ? (HL[i] % 64) : 64;
  uint64_t inter = (hl < 64) ? (((1 << hl) - 1) & gpath[0]) : gpath[0];
  uint64_t res = 0;
  inter ^= (pc >> (i)) ^ (pc); // ^ (pc >> HL[i]); // ^ (pc << (HL[i]%32));

  for (int t = 0; t < 8; t++)
    {
      res ^= inter;
      inter ^= ((inter & 15) << 16);
      inter >>= (LOGBANK - ((NHIST - i + LOGBANK - 1) % (LOGBANK - 1)));
    }
  hl = (hl < (HL[NHIST] + 1) / 2) ? hl : ((HL[NHIST] + 1) / 2);

  inter ^= (hl < 64) ? (((1 << hl) - 1) & gtargeth) : gtargeth;
  inter ^= (gtargeth | ((1 << hl)-1));
  for (int t = 0; t <= hl / LOGBANK; t++)
    {
      res ^= inter;
      inter ^= ((inter & 15) << 16);
      inter >>= LOGBANK;
    }

  if (HL[i] >= 64)
    {
      int REMAIN = HL[i] - 64;
      hl = REMAIN;
      int PT = 1;

      while (REMAIN > 0)
	{


	  inter ^= ((hl < 64) ? (((1 << hl) - 1) & gpath[PT]) : gpath[PT]);
	  for (int t = 0; t < 8; t++)
	    {
	      res ^= inter;
	      inter ^= ((inter & 15) << 16);

	      inter >>= (LOGBANK -
			 ((NHIST - i + LOGBANK - 1) % (LOGBANK - 1)));

	    }
	  REMAIN = REMAIN - 64;
	  PT++;
	}
    }
  return ((uint32_t) res & (BANKSIZE - 1));
}



//tags for VTAGE: just another complex hash function "orthogonal" to the index function
uint32_t
gtag (int i, uint64_t pc)
{
  int hl = (HL[i] < 64) ? (HL[i] % 64) : 64;
  uint64_t inter = (hl < 64) ? (((1 << hl) - 1) & gpath[0]) : gpath[0];

  uint64_t res = 0;
  inter ^= ((pc >> (i)) ^ (pc >> (5 + i)) ^ (pc) ^ (pc>>HL[i]%64));
  for (int t = 0; t < 8; t++)
    {
      res ^= inter;
      inter ^= ((inter & 31) << 14);
      inter >>= (LOGBANK - ((NHIST - i + LOGBANK - 2) % (LOGBANK - 1)));
    }
  hl = (hl < (HL[NHIST] + 1) / 2) ? hl : ((HL[NHIST] + 1) / 2);
  inter ^= ((hl < 64) ? (((1 << hl) - 1) & gtargeth) : gtargeth);
  for (int t = 0; t <= hl / TAGWIDTH; t++)
    {
      res ^= inter;
      inter ^= ((inter & 15) << 16);
      inter >>= TAGWIDTH;
    }

  if (HL[i] >= 64)
    {
      int REMAIN = HL[i] - 64;
      hl = REMAIN;
      int PT = 1;

      while (REMAIN > 0)
	{


	  inter ^= ((hl < 64) ? (((1 << hl) - 1) & gpath[PT]) : gpath[PT]);
	  for (int t = 0; t < 8; t++)
	    {
	      res ^= inter;
	      inter ^= ((inter & 31) << 14);
	      inter >>= (TAGWIDTH - (NHIST - i - 1));


	    }
	  REMAIN = REMAIN - 64;
	  PT++;
	}
    }

  return ((uint32_t) res & ((1 << TAGWIDTH) - 1));
}






////// for managing speculative state and forwarding information to the back-end
struct ForUpdate
{
  bool predvtage;
  bool predstride;
  bool prediction_result;
  uint8_t todo;
  uint64_t pc;
  uint32_t GI[NHIST + 1];
  uint32_t GTAG[NHIST + 1];
  int B[NBWAYSTR];
  int TAGSTR[NBWAYSTR];
  int STHIT;
  int HitBank;
  int8_t INSTTYPE;
  int8_t NbOperand;

};

#define MAXINFLIGHT 512
static ForUpdate Update[MAXINFLIGHT];	// there may be 512 instructions inflight
