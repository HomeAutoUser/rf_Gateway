#include "config.h"

#ifdef CC110x
#include "cc110x.h"
#elif RFM69
#include "rfm69.h"
#endif

/*
  Register array with all assignments ( define the array of structs )
  [x] is de array pos of Registers
  access to reg value´s                     -> Registers[x].reg_val[x]
  access to reg length                      -> Registers[x].length
  access to reg name TXT                    -> Registers[x].name
  access to PKTLEN from protocol            -> Registers[x].PKTLEN
  Registername        Registerlength,   Name from           Packet
  to access values                      register to View    length
  reg_val             length            name                PKTLEN
*/

struct Data Registers[] = {
  { Config_Default, sizeof(Config_Default) / sizeof(Config_Default[0]), "Chip factory default", 32  },
  { Config_User, REGISTER_MAX, "Chip user setting", 0 },
#ifdef OOK_MU_433
  { Config_OOK_MU_433,  sizeof(Config_OOK_MU_433) / sizeof(Config_OOK_MU_433[0]), "OOK_MU_433", 5  },
#endif
#ifdef Avantek
  { Config_Avantek, sizeof(Config_Avantek) / sizeof(Config_Avantek[0]), "Avantek",  8   },
#endif
#ifdef Bresser_5in1
  { Config_Bresser_5in1,  sizeof(Config_Bresser_5in1) / sizeof(Config_Bresser_5in1[0]), "Bresser_5in1", 26  },
#endif
#ifdef Bresser_6in1
  { Config_Bresser_6in1,  sizeof(Config_Bresser_6in1) / sizeof(Config_Bresser_6in1[0]), "Bresser_6in1", 18  },
#endif
#ifdef Bresser_7in1
  { Config_Bresser_7in1,  sizeof(Config_Bresser_7in1) / sizeof(Config_Bresser_7in1[0]), "Bresser_7in1", 23  },
#endif
#ifdef Fine_Offset_WH51_434
  { Config_Fine_Offset_WH51_434,  sizeof(Config_Fine_Offset_WH51_434) / sizeof(Config_Fine_Offset_WH51_434[0]), "Fine_Offset_WH51_434", 14  },
#endif
#ifdef Fine_Offset_WH51_868
  { Config_Fine_Offset_WH51_868,  sizeof(Config_Fine_Offset_WH51_868) / sizeof(Config_Fine_Offset_WH51_868[0]), "Fine_Offset_WH51_868", 14  },
#endif
#ifdef Fine_Offset_WH57_434
  { Config_Fine_Offset_WH57_434,  sizeof(Config_Fine_Offset_WH57_434) / sizeof(Config_Fine_Offset_WH57_434[0]), "Fine_Offset_WH57_434", 9  },
#endif
#ifdef Fine_Offset_WH57_868
  { Config_Fine_Offset_WH57_868,  sizeof(Config_Fine_Offset_WH57_868) / sizeof(Config_Fine_Offset_WH57_868[0]), "Fine_Offset_WH57_868", 9  },
#endif
#ifdef Inkbird_IBS_P01R
  { Config_Inkbird_IBS_P01R,  sizeof(Config_Inkbird_IBS_P01R) / sizeof(Config_Inkbird_IBS_P01R[0]), "Inkbird_IBS_P01R", 18  },
#endif
#ifdef KOPP_FC
  { Config_KOPP_FC,  sizeof(Config_KOPP_FC) / sizeof(Config_KOPP_FC[0]), "KOPP_FC", 15  },
#endif
#ifdef Lacrosse_mode1
  { Config_Lacrosse_mode1,  sizeof(Config_Lacrosse_mode1) / sizeof(Config_Lacrosse_mode1[0]), "Lacrosse_mode1", 5   },
#endif
#ifdef Lacrosse_mode2
  { Config_Lacrosse_mode2,  sizeof(Config_Lacrosse_mode2) / sizeof(Config_Lacrosse_mode2[0]), "Lacrosse_mode2", 5   },
#endif
#ifdef PCA301
  { Config_PCA301,  sizeof(Config_PCA301) / sizeof(Config_PCA301[0]), "PCA301", 32  },
#endif
#ifdef Rojaflex
  { Config_Rojaflex,  sizeof(Config_Rojaflex) / sizeof(Config_Rojaflex[0]), "Rojaflex", 9  },
#endif
#ifdef WMBus_S
  { Config_WMBus_S,  sizeof(Config_WMBus_S) / sizeof(Config_WMBus_S[0]), "WMBus_S", 32  },
#endif
#ifdef WMBus_T
  { Config_WMBus_T,  sizeof(Config_WMBus_T) / sizeof(Config_WMBus_T[0]), "WMBus_T", 32  },
#endif
#ifdef X_Sense
  { Config_X_Sense,  sizeof(Config_X_Sense) / sizeof(Config_X_Sense[0]), "X_Sense", 6  },
#endif


  /* under development | CC110x */
#ifdef CC110x
#ifdef HomeMatic
  { Config_HomeMatic,  sizeof(Config_HomeMatic) / sizeof(Config_HomeMatic[0]), "under dev - HomeMatic", 12  },
#endif
#ifdef Lacrosse_mode3
  { Config_Lacrosse_mode3,  sizeof(Config_Lacrosse_mode3) / sizeof(Config_Lacrosse_mode3[0]), "Lacrosse_mode3", 5   },
#endif
#ifdef Max
  { Config_MAX,  sizeof(Config_MAX) / sizeof(Config_MAX[0]), "under dev - MAX", 12  },
#endif
#endif
  /* under development END */
};
