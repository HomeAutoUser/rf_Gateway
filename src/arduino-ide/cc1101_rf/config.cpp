#include "config.h"

#ifdef CC110x
#include "cc110x.h"
#elif RFM69
#include "rfm69.h"
#endif

/*
  Register array with all assignments ( define the array of structs )
  [x] is de array pos of Registers
  access to reg value´s                     -> myArraySRAM.regVal - ESP pointer to register values
  access to reg name TXT                    -> String getModeName(ReceiveModeNr) - pointer to register name strings
  access to PKTLEN from protocol            -> myArraySRAM.PKTLEN - packet lenght to read from FIFO
  Registername, Name,    Packet lenght
  regVal,       regName, PKTLEN
*/
const struct Data Registers[] PROGMEM = {
  { Config_Default, Name_Default, 0  },
  { Config_User, Name_User, 0 },
#ifdef OOK_MU_433
  { Config_OOK_MU_433, Name_OOK_MU_433, 5  },
#endif
#ifdef Avantek
  { Config_Avantek, Name_Avantek,  8   },
#endif
#ifdef Bresser_5in1
  { Config_Bresser_5in1, Name_Bresser_5in1, 26  },
#endif
#ifdef Bresser_6in1
  { Config_Bresser_6in1, Name_Bresser_6in1, 18  },
#endif
#ifdef Bresser_7in1
  { Config_Bresser_7in1, Name_Bresser_7in1, 23  },
#endif
#ifdef Fine_Offset_WH51_434
  { Config_Fine_Offset_WH51_434, Name_Fine_Offset_WH51_434, 14  },
#endif
#ifdef Fine_Offset_WH51_868
  { Config_Fine_Offset_WH51_868, Name_Fine_Offset_WH51_868, 14  },
#endif
#ifdef Fine_Offset_WH57_434
  { Config_Fine_Offset_WH57_434, Name_Fine_Offset_WH57_434, 9  },
#endif
#ifdef Fine_Offset_WH57_868
  { Config_Fine_Offset_WH57_868, Name_Fine_Offset_WH57_868, 9  },
#endif
#ifdef Inkbird_IBS_P01R
  { Config_Inkbird_IBS_P01R, Name_Inkbird_IBS_P01R, 18  },
#endif
#ifdef KOPP_FC
  { Config_KOPP_FC, Name_KOPP_FC, 15  },
#endif
#ifdef Lacrosse_mode1
  { Config_Lacrosse_mode1, Name_Lacrosse_mode1, 5   },
#endif
#ifdef Lacrosse_mode2
  { Config_Lacrosse_mode2, Name_Lacrosse_mode2, 5   },
#endif
#ifdef PCA301
  { Config_PCA301, Name_PCA301, 32  },
#endif
#ifdef Rojaflex
  { Config_Rojaflex, Name_Rojaflex, 9  },
#endif
#ifdef WMBus_S
  { Config_WMBus_S, Name_WMBus_S, 32  },
#endif
#ifdef WMBus_T
  { Config_WMBus_T, Name_WMBus_T, 32  },
#endif
#ifdef X_Sense
  { Config_X_Sense, Name_X_Sense, 6  },
#endif


  /* under development | CC110x */
#ifdef CC110x
#ifdef HomeMatic
  { Config_HomeMatic, REGISTER_MAX, "under dev - HomeMatic", 12  },
#endif
#ifdef Lacrosse_mode3
  { Config_Lacrosse_mode3, REGISTER_MAX, "Lacrosse_mode3", 5   },
#endif
#ifdef Max
  { Config_MAX, REGISTER_MAX, "under dev - MAX", 12  },
#endif
#endif
  /* under development END */
};
