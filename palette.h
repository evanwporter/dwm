#ifndef PALETTE_H
#define PALETTE_H

// TODO: Combine

/* Background colors */
#define DWM_BG_DIM "#1b1b1b"
#define DWM_BG0 "#282828"
#define DWM_BG1 "#32302f"
#define DWM_BG2 "#32302f"
#define DWM_BG3 "#45403d"
#define DWM_BG4 "#45403d"
#define DWM_BG5 "#5a524c"

#define DWM_BG_STATUSLINE1 "#32302f"
#define DWM_BG_STATUSLINE2 "#3a3735"
#define DWM_BG_STATUSLINE3 "#504945"

#define DWM_BG_VISUAL_RED "#4c3432"
#define DWM_BG_VISUAL_YELLOW "#4f422e"
#define DWM_BG_VISUAL_GREEN "#3b4439"
#define DWM_BG_VISUAL_BLUE "#374141"
#define DWM_BG_VISUAL_PURPLE "#443840"

#define DWM_BG_DIFF_RED "#402120"
#define DWM_BG_DIFF_GREEN "#34381b"
#define DWM_BG_DIFF_BLUE "#0e363e"

#define DWM_BG_CURRENT_WORD "#3c3836"

/* Foreground colors */
#define DWM_NORM_FG "#ebdbb2"
#define DWM_NORM_BG DWM_BG0
#define DWM_NORM_BORDER "#3c3836"
#define DWM_SEL_FG "#fbf1c7"
#define DWM_SEL_BG "#e78a3e"
#define DWM_SEL_BORDER DWM_SEL_BG
#define DWM_STATUS_FG DWM_NORM_FG
#define DWM_STATUS_BG DWM_NORM_BG
#define DWM_STATUS_BORDER DWM_NORM_BORDER

#define DWM_FG0 "#d4be98"
#define DWM_FG1 "#ddc7a1"

#define DWM_RED "#ea6962"
#define DWM_ORANGE "#e78a4e"
#define DWM_YELLOW "#d8a657"
#define DWM_GREEN "#a9b665"
#define DWM_AQUA "#89b482"
#define DWM_BLUE "#7daea3"
#define DWM_PURPLE "#d3869b"

#define DWM_WHITE DWM_FG0

#define DWM_BG_RED "#ea6962"
#define DWM_BG_GREEN "#a9b665"
#define DWM_BG_YELLOW "#d8a657"

#define DWM_GRAY_DARK "#665c54"
#define DWM_GRAY "#a89984"
#define DWM_GRAY_BRIGHT "#928374"

/* dmenu colors */
#define DMENU_NORM_FG DWM_NORM_FG
#define DMENU_NORM_BG DWM_NORM_BG
#define DMENU_SEL_FG DWM_SEL_FG
#define DMENU_SEL_BG DWM_SEL_BG
#define DMENU_OUT_FG DWM_NORM_FG
#define DMENU_OUT_BG DWM_NORM_BG

/* st colors */
#define ST_BLACK "#665c54"
#define ST_RED "#ea6962"
#define ST_GREEN "#a9b665"
#define ST_YELLOW "#e78a4e"
#define ST_BLUE "#7daea3"
#define ST_MAGENTA "#d3869b"
#define ST_CYAN "#89b482"
#define ST_WHITE "#d4be98"
#define ST_BRIGHT_BLACK "#928374"
#define ST_BRIGHT_YELLOW "#d8a657"
#define ST_CURSOR "#a89984"
#define ST_CURSOR_TEXT "#282828"
#define ST_BG ST_CURSOR_TEXT
#define ST_FG ST_WHITE

/* slock colors */
#define SLOCK_INIT "black"
#define SLOCK_INPUT "#005577"
#define SLOCK_FAILED "#CC3333"

/* dwmblocks colors */
#define DWM_BLOCKS_BLACK "#222526"
#define DWM_BLOCKS_GREEN "#89b482"
#define DWM_BLOCKS_WHITE "#c7b89d"
#define DWM_BLOCKS_GREY "#2b2e2f"
#define DWM_BLOCKS_BLUE "#6f8faf"
#define DWM_BLOCKS_RED "#ec6b64"
#define DWM_BLOCKS_DARKBLUE "#6080a0"
#define DWM_BLOCKS_YELLOW "#d8a657"

/* dwmblocks color helpers */
#define DWM_BLOCKS_FG(color) "^c" color "^"
#define DWM_BLOCKS_BG(color) "^b" color "^"
#define DWM_BLOCKS_RESET "^d^"

#endif /* PALETTE_H */
