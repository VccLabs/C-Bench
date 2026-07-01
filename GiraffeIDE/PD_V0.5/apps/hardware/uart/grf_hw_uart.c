#include "../grf_hw.h"
#if GRF_HW_ENABLE
#include "grf_hw_uart.h"

static grf_drv_t *drv_uart = NULL;

#define LBL_VOLT 1
#define ARC_VOLT 7 /* <- your arc's Control ID from view1.h */

#define LBL_CURR 3
#define LBL_POWER 5
#define LBL_OUT 11 /* view1 output toggle label (VIEW1_LABEL7_ID) */

#define LBL_OUT 11  /* view1 output toggle label (VIEW1_LABEL7_ID) */
#define LBL_WH 4    /* view1 session energy Wh   (VIEW1_LABEL3_ID)  */
#define BTN_RST 16  /* view1 session reset btn   (VIEW1_LABEL13_ID) */
#define RST_TINT 25 /* view1 reset press overlay (VIEW1_LABEL21_ID) */

#define MAX_PROF 13
typedef struct
{
    u16 type, vmin, vmax, imax;
} prof_t;
static prof_t g_prof[MAX_PROF];
static u8 g_prof_n = 0;

#define LBL_EMPTY1 80 /* label78 - "No profiles..." view2*/
#define LBL_EMPTY2 81 /* label79 - subtitle view2*/
#define LBL_STAT   98 /* label91 - source summary / empty prompt view2 */

/* view2 adjust panel + Use button (PPS/AVS fine-adjust) */
#define ADJ_CONT 82 /* container1            */
#define ADJ_SV 83   /* slider0 - set voltage  -> reg 0x0020 (mV) */
#define ADJ_SC 87   /* slider1 - current limit-> reg 0x0021 (mA) */
#define ADJ_LV 88   /* label82 - voltage value */
#define ADJ_LC 89   /* label83 - current value */
#define BTN_USE 90  /* label84 - Use/apply button */
#define SEL_BOX 84  /* <- set to the selbox outline control's ID */

/* ───────── THEME COLOR TABLE ─────────
 * g_dark holds the 0x0039 reg value: 0 = dark, 1 = light.
 * That doubles as the column index into THEME[role][0=dark | 1=light]. */
enum {
    TC_BG, TC_SURF, TC_SURF2, TC_TRACK, TC_TXT, TC_TXT2, TC_TXT3,
	TC_GREEN, TC_RED, TC_ORANGE, TC_BLUE, TC_CHIP, TC_N
};
static const u32 THEME[TC_N][2] = {
    /*               dark                          light                       */
    /* TC_BG     */ { GRF_COLOR_GET(0x00,0x00,0x00), GRF_COLOR_GET(0xF2,0xF2,0xF7) },
    /* TC_SURF   */ { GRF_COLOR_GET(0x1C,0x1C,0x1E), GRF_COLOR_GET(0xFF,0xFF,0xFF) },
    /* TC_SURF2  */ { GRF_COLOR_GET(0x2C,0x2C,0x2E), GRF_COLOR_GET(0xE5,0xE5,0xEA) },
    /* TC_TRACK  */ { GRF_COLOR_GET(0x44,0x44,0x46), GRF_COLOR_GET(0xC7,0xC7,0xCC) },
    /* TC_TXT    */ { GRF_COLOR_GET(0xFF,0xFF,0xFF), GRF_COLOR_GET(0x00,0x00,0x00) },
    /* TC_TXT2   */ { GRF_COLOR_GET(0x8E,0x8E,0x93), GRF_COLOR_GET(0x6C,0x6C,0x70) },
    /* TC_TXT3   */ { GRF_COLOR_GET(0x47,0x47,0x47), GRF_COLOR_GET(0xC7,0xC7,0xCC) },
    /* TC_GREEN  */ { GRF_COLOR_GET(0x30,0xD1,0x58), GRF_COLOR_GET(0x34,0xC7,0x59) },
    /* TC_RED    */ { GRF_COLOR_GET(0xFF,0x45,0x3A), GRF_COLOR_GET(0xFF,0x3B,0x30) },
    /* TC_ORANGE */ { GRF_COLOR_GET(0xFF,0x9F,0x0A), GRF_COLOR_GET(0xFF,0x95,0x00) },
	/* TC_BLUE   */ { GRF_COLOR_GET(0x0A,0x84,0xFF), GRF_COLOR_GET(0x00,0x7A,0xFF) },
	/* TC_CHIP   */ { GRF_COLOR_GET(0x2C,0x2C,0x2E), GRF_COLOR_GET(0xFF,0xFF,0xFF) },
	};
static u8 g_dark = 0;
#define TCOL(role) (THEME[(role)][g_dark])          /* current color for a role */
#define THEME_BG(ctrl, role)  grf_ctrl_style_set_bg_color((ctrl), TCOL(role), 0)
#define THEME_TXT(ctrl, role) grf_label_set_txt_color((ctrl), TCOL(role))
/* selected-row card fill: orangy dark in dark mode, orangy white in light mode */
#define SEL_TINT (g_dark ? GRF_COLOR_GET(0xFF, 0xEC, 0xD1) : GRF_COLOR_GET(0x3A, 0x2A, 0x10))
static void boot_state_paint(u8 last_used);  /* fwd decl: used by theme_apply_view4 */
static void theme_state_paint(void);         /* fwd decl: used by theme_apply_view4 */
static u8 g_v4_boot = 0;                      /* shadow of reg 0x0031 (0=Off, 1=Last used) */
extern u8 g_v4_boot;                          /* fwd: boot-state shadow (defined below) */

/* per-row Control IDs: {badge, volt, meta, curr, check} */
enum
{
    COL_BADGE,
    COL_VOLT,
    COL_META,
    COL_CURR,
    COL_CHECK,
    COL_BG
};
static const u16 ROW_ID[MAX_PROF][6] = {
    /* {badge, volt, meta, curr, check, bg} */
    {6, 2, 3, 4, 5, 67},      /* row 0  */
    {10, 9, 8, 7, 1, 68},     /* row 1  */
    {12, 11, 13, 14, 15, 79}, /* row 2  */
    {26, 27, 28, 29, 30, 78}, /* row 3  */
    {25, 24, 18, 22, 21, 77}, /* row 4  */
    {16, 17, 23, 19, 20, 76}, /* row 5  */
    {55, 54, 43, 42, 31, 69}, /* row 6  */
    {56, 53, 44, 41, 32, 70}, /* row 7  */
    {57, 52, 45, 40, 33, 71}, /* row 8  */
    {58, 51, 46, 39, 34, 72}, /* row 9  */
    {59, 50, 47, 38, 35, 73}, /* row 10 */
    {60, 49, 48, 37, 36, 75}, /* row 11 */
    {65, 64, 63, 62, 61, 74}, /* row 12 */
};
static void show_row(u8 i, u8 vis)
{
    for (u8 k = 0; k < 6; k++)
        grf_ctrl_set_hidden(GCL(GRF_VIEW2_ID, ROW_ID[i][k]), vis ? 0 : 1);
}

static u8 g_sel = 0xFF; /* selected row, 0xFF = none */

static void highlight_row(u8 i, u8 on)
{
    grf_ctrl_t *bg = GCL(GRF_VIEW2_ID, ROW_ID[i][COL_BG]);
    grf_ctrl_t *box = GCL(GRF_VIEW2_ID, SEL_BOX);
    /* check mark */
    grf_ctrl_set_hidden(GCL(GRF_VIEW2_ID, ROW_ID[i][COL_CHECK]), on ? 0 : 1);
    /* background chip: orange tint when selected, default card when not */
    grf_ctrl_style_set_bg_color(bg,
                                        on ? SEL_TINT : TCOL(TC_SURF), 0);
    /* #ff9f0a border: move the single outline box over the selected row */
    if (on)
    {
        grf_ctrl_set_pos(box, grf_ctrl_get_x(bg), grf_ctrl_get_y(bg));
        grf_ctrl_set_size(box, grf_ctrl_get_width(bg), grf_ctrl_get_height(bg));
        grf_ctrl_move_forground(box);
        grf_ctrl_set_hidden(box, 0);
    }
    else
    {
        grf_ctrl_set_hidden(box, 1);
    }
}

static u8   g_use_en = 0;                 /* shadow: 1 = selected/orange, 0 = "Select a rail" */
static char g_use_txt[40] = "Select a rail";
static void use_btn_paint(void)           /* paint Use button from shadow + theme */
{
    if (g_prof_n == 0)                    /* no source -> hide the button entirely */
    {
        grf_ctrl_set_hidden(GCL(GRF_VIEW2_ID, BTN_USE), 1);
        return;
    }
    grf_ctrl_set_hidden(GCL(GRF_VIEW2_ID, BTN_USE), 0);
    grf_label_set_txt(GCL(GRF_VIEW2_ID, BTN_USE), g_use_txt);
    /* selected: fixed orange (same both themes). idle: themed surface/text. */
    grf_ctrl_style_set_bg_color(GCL(GRF_VIEW2_ID, BTN_USE),
                                g_use_en ? GRF_COLOR_GET(0xFF, 0x9F, 0x0A) : TCOL(TC_SURF2), 0);
    grf_label_set_txt_color(GCL(GRF_VIEW2_ID, BTN_USE),
                            g_use_en ? GRF_COLOR_GET(0x23, 0x13, 0x00) : TCOL(TC_TXT2));
}
static void use_btn_set(u8 enabled, const char *txt)
{
    g_use_en = enabled;
    snprintf(g_use_txt, sizeof(g_use_txt), "%s", txt);
    use_btn_paint();
}

static void adj_labels(u16 mv, u16 ma)
{
    char b[16];
    snprintf(b, sizeof(b), "%u.%02u V", mv / 1000, (mv % 1000) / 10);
    grf_label_set_txt(GCL(GRF_VIEW2_ID, ADJ_LV), b);
    snprintf(b, sizeof(b), "%u.%u A", ma / 1000, (ma % 1000) / 100);
    grf_label_set_txt(GCL(GRF_VIEW2_ID, ADJ_LC), b);
}

#define LBL_PROF 22 /* view1 active profile (VIEW1_LABEL18_ID) */
static u8 g_ap_type = 0;
static u16 g_ap_mV = 0;
static void ap_paint(void)
{
    const char *nm = (g_ap_type == 1) ? "Fixed" : (g_ap_type == 2) ? "PPS"
                                              : (g_ap_type == 3)   ? "AVS"
                                              : (g_ap_type == 4)   ? "EPR"
                                                                   : 0;
    if (!nm)
    {
        grf_label_set_txt(GCL(GRF_VIEW1_ID, LBL_PROF), "—");
        return;
    }
    char b[24];
    snprintf(b, sizeof(b), "%s %u.%02u V", nm, g_ap_mV / 1000, (g_ap_mV % 1000) / 10);
    grf_label_set_txt(GCL(GRF_VIEW1_ID, LBL_PROF), b);
}

#define ADJ_Y_SHOWN 405  /* must match container1's Y in the IDE */
#define ADJ_Y_HIDDEN 720 /* parked just below the 720px screen */
static u8 g_panel_up = 0;
static u8 g_arm_pending = 0; /* set on apply, consumed in view1_entry */
static u8 g_out_on = 0;      /* shadow of real output state (RP reg 0x0016) */
static u16 g_arc = 0;        /* shadow of eased arc value (RP reg 0x001B) */
static u8 g_applied = 0xFF;  /* last-applied list position, re-highlighted on return */

static void adj_panel(u8 show)
{
    if (show == g_panel_up)
        return;
    grf_ctrl_set_hidden(GCL(GRF_VIEW2_ID, ADJ_CONT), show ? 0 : 1);
    g_panel_up = show;
}

static void update_adjust(u8 i)
{
    prof_t *p = &g_prof[i];
    u8 range = (p->type == 1 || p->type == 2) && (p->vmin != p->vmax);
    char b[20];
    if (range)
    {
        grf_slider_set_range(GCL(GRF_VIEW2_ID, ADJ_SV), p->vmin, p->vmax);
        grf_slider_set_value(GCL(GRF_VIEW2_ID, ADJ_SV), p->vmax);
        grf_slider_set_range(GCL(GRF_VIEW2_ID, ADJ_SC), 0, p->imax);
        grf_slider_set_value(GCL(GRF_VIEW2_ID, ADJ_SC), p->imax);
        adj_labels(p->vmax, p->imax);
        adj_panel(1); /* slide up */
        snprintf(b, sizeof(b), "Use %u.%02u V", p->vmax / 1000, (p->vmax % 1000) / 10);
    }
    else
    {
        adj_panel(0); /* slide down */
        snprintf(b, sizeof(b), "Use %u.%02u V", p->vmin / 1000, (p->vmin % 1000) / 10);
    }
    use_btn_set(1, b);
}

void select_row_by_bg(grf_ctrl_t *ctrl)
{
    if (g_panel_up)
    { /* panel open: tap outside just dismisses it */
        if (g_sel != 0xFF)
        {
            highlight_row(g_sel, 0);
            g_sel = 0xFF;
        }
        adj_panel(0);
        use_btn_set(0, "Select a rail");
        return; /* don't select the tapped card */
    }
    for (u8 i = 0; i < MAX_PROF; i++)
    {
        if (GCL(GRF_VIEW2_ID, ROW_ID[i][COL_BG]) == ctrl)
        {
            if (g_sel != 0xFF)
                highlight_row(g_sel, 0); /* clear previous */
            g_sel = i;
            highlight_row(i, 1); /* highlight new */
            update_adjust(i);    /* drive panel + Use btn */
            return;
        }
    }
}

void view2_slider_changed(u8 which)
{
    u16 mv = grf_slider_get_value(GCL(GRF_VIEW2_ID, ADJ_SV));
    u16 ma = grf_slider_get_value(GCL(GRF_VIEW2_ID, ADJ_SC));
    char b[20];
    (void)which;
    adj_labels(mv, ma);
    snprintf(b, sizeof(b), "Use %u.%02u V", mv / 1000, (mv % 1000) / 10);
    use_btn_set(1, b);
}

void view2_use_apply(void)
{
    prof_t *p;
    if (g_sel == 0xFF)
        return; /* nothing selected */
    p = &g_prof[g_sel];
    if ((p->type == 1 || p->type == 2) && (p->vmin != p->vmax))
    {
        grf_reg_set(0x0020, grf_slider_get_value(GCL(GRF_VIEW2_ID, ADJ_SV)));
        grf_reg_com_send(0x0020, 1); /* latch mV first */
        grf_reg_set(0x0021, grf_slider_get_value(GCL(GRF_VIEW2_ID, ADJ_SC)));
        grf_reg_com_send(0x0021, 1); /* then mA */
    }
    grf_reg_set(0x0023, g_sel);
    grf_reg_com_send(0x0023, 1); /* apply: RP maps pos -> PDO, arms */
    use_btn_set(1, "Applied");
    g_applied = g_sel; /* remember active rail to re-highlight on return */
    g_arm_pending = 1;
    grf_view_set_dis_view_anim(GRF_VIEW1_ID, GRF_SCR_LOAD_ANIM_MOVE_RIGHT,
                               250, 0, GRF_ANIM_PATH_END_SLOW);
}

static void view1_set_output_btn(u8 on) /* drive output label from real state */
{
    grf_label_set_txt(GCL(GRF_VIEW1_ID, LBL_OUT), on ? "Turn output off" : "Turn output on");
    grf_ctrl_style_set_bg_color(GCL(GRF_VIEW1_ID, LBL_OUT),
                                on ? GRF_COLOR_GET(0xFF, 0x45, 0x3A) /* red  #ff453a */
                                   : GRF_COLOR_GET(0x30, 0xD1, 0x58),
                                0); /* green #30D158 */
    grf_label_set_txt_color(GCL(GRF_VIEW1_ID, LBL_OUT), GRF_COLOR_GET(0xFF, 0xFF, 0xFF));
}

void view1_sync_armed(void) /* called from view1_entry */
{
	g_arm_pending = 0;              /* no longer drives the visual */
	    view1_set_output_btn(g_out_on); /* reflect the real state instead */
	    grf_arc_set_value(GCL(GRF_VIEW1_ID, ARC_VOLT), g_arc); /* restore eased arc after view reset */
}

void view1_toggle_output(void) /* label7 click: request opposite of real state */
{
    u16 want = g_out_on ? 0 : 1;
    grf_reg_set(0x0022, want);
    grf_reg_com_send(0x0022, 1); /* RP flips output, pushes 0x0016 back to repaint */
}

void view1_reset_press(u8 down) /* show/hide the reset press-tint overlay */
{
    grf_ctrl_set_hidden(GCL(GRF_VIEW1_ID, RST_TINT), down ? 0 : 1);
}
void view1_reset_session(void) /* reset button -> tell RP to zero the trip */
{
    grf_reg_set(0x0025, 1);
    grf_reg_com_send(0x0025, 1);
}

/* ── view1 (Monitor) themed control IDs ── */
#define V1_VVAL    1   /* label0  — voltage value      txt */
#define V1_LBL25  29   /* label25 —                    txt */
#define V1_LBL26  30   /* label26 —                    txt2 */
#define V1_LBL19  23   /* label19 —                    txt2 */
#define V1_LBL20  24   /* label20 —                    green */
#define V1_PROF   22   /* label18 — active profile     txt2 */
#define V1_CARD1  19   /* label16 — card surface       surf */
#define V1_CARD2  20   /* label17 — card surface       surf */
#define V1_CARD3   2   /* label1  — card surface       surf */
#define V1_RST    16   /* label13 — reset chip         surf2 + txt2 */
#define V1_ARROW  21   /* image1  — reset arrow icon   img swap */
#define V1_RSTHL  25   /* label21 — reset press tint   txt (press dir) */
#define V1_ELAP   26   /* label22 — elapsed value      txt */
#define V1_NAV    10   /* image0  — nav bar image      img swap */

static void theme_apply_view1(void)
{
    grf_view_set_bgcolor(GRF_VIEW1_ID, TCOL(TC_BG));        /* screen bg */
    THEME_TXT(GCL(GRF_VIEW1_ID, V1_VVAL), TC_TXT);
    THEME_TXT(GCL(GRF_VIEW1_ID, V1_LBL25), TC_TXT);
    THEME_TXT(GCL(GRF_VIEW1_ID, V1_LBL26), TC_TXT2);
    THEME_TXT(GCL(GRF_VIEW1_ID, V1_LBL19), TC_TXT2);
    THEME_TXT(GCL(GRF_VIEW1_ID, V1_LBL20), TC_GREEN);
    THEME_TXT(GCL(GRF_VIEW1_ID, V1_PROF), TC_TXT2);
    THEME_BG(GCL(GRF_VIEW1_ID, V1_CARD1), TC_SURF);
    THEME_BG(GCL(GRF_VIEW1_ID, V1_CARD2), TC_SURF);
    THEME_BG(GCL(GRF_VIEW1_ID, V1_CARD3), TC_SURF);
    {   /* arc voltage ring — track is a line (part 0): color,width,opa,rounded */
            grf_line_disp_t arc_bg = { TCOL(TC_TRACK), 27, 255, 12 };
            grf_arc_set_dis(GCL(GRF_VIEW1_ID, ARC_VOLT), 0, arc_bg);
        }
        /* session: reset chip + elapsed */
        THEME_BG (GCL(GRF_VIEW1_ID, V1_RST),   TC_SURF2);
        THEME_TXT(GCL(GRF_VIEW1_ID, V1_RST),   TC_TXT2);
        THEME_BG (GCL(GRF_VIEW1_ID, V1_RSTHL), TC_TXT);     /* press-tint color; opacity from IDE */
        THEME_TXT(GCL(GRF_VIEW1_ID, V1_ELAP),  TC_TXT);
        /* image swaps (dark vs light asset) */
        grf_img_set_src(GCL(GRF_VIEW1_ID, V1_ARROW),
                        g_dark ? "arrow-light.png"        : "arrow-dark.png");
        grf_img_set_src(GCL(GRF_VIEW1_ID, V1_NAV),
                        g_dark ? "nav-monitor-light.png"  : "nav-monitor.png");
    }

/* ── view2 (Profiles) themed control IDs ── */
#define V2_BRAND  91   /* label85 — "C-Bench"     txt  */
#define V2_SUB    94   /* label87 — "· Profiles"  txt2 */
#define V2_TITLE  97   /* label90 — "Profiles"    txt  */
#define V2_NAV    92   /* image0  — nav bar image  img swap */
/* status line label91 (id 98) reuses LBL_STAT */

static void row_badge_colors(u8 i)   /* per-type badge bg/txt */
{
    grf_ctrl_t *b = GCL(GRF_VIEW2_ID, ROW_ID[i][COL_BADGE]);
    switch (g_prof[i].type)
    {
    case 1: grf_ctrl_style_set_bg_color(b, GRF_COLOR_GET(0x64,0xD2,0xFF), 0); /* PPS */
            grf_label_set_txt_color(b, GRF_COLOR_GET(0x06,0x2A,0x30)); break;
    case 2: grf_ctrl_style_set_bg_color(b, GRF_COLOR_GET(0xFF,0x9F,0x0A), 0); /* AVS */
            grf_label_set_txt_color(b, GRF_COLOR_GET(0x2A,0x18,0x00)); break;
    case 3: grf_ctrl_style_set_bg_color(b, GRF_COLOR_GET(0xBF,0x5A,0xF2), 0); /* EPR */
            grf_label_set_txt_color(b, GRF_COLOR_GET(0x1E,0x0C,0x33)); break;
    default: grf_ctrl_style_set_bg_color(b, TCOL(TC_CHIP), 0);                /* FIX */
            grf_label_set_txt_color(b, TCOL(TC_TXT2)); break;
    }
}

static void view2_paint_cards(void)   /* single row-color authority: render + entry + theme */
{
    for (u8 i = 0; i < MAX_PROF; i++)
    {
    	grf_ctrl_style_set_bg_color(GCL(GRF_VIEW2_ID, ROW_ID[i][COL_BG]),
    	            (i == g_sel) ? SEL_TINT : TCOL(TC_SURF), 0);
        if (i >= g_prof_n) continue;
        grf_label_set_txt_color(GCL(GRF_VIEW2_ID, ROW_ID[i][COL_VOLT]),  TCOL(TC_TXT));                /* range "5.0-11.0 V" */
        grf_label_set_txt_color(GCL(GRF_VIEW2_ID, ROW_ID[i][COL_META]),  TCOL(TC_TXT2));               /* "adjustable rail" */
        grf_label_set_txt_color(GCL(GRF_VIEW2_ID, ROW_ID[i][COL_CURR]),  GRF_COLOR_GET(0x64,0xD2,0xFF)); /* current = blue, both themes */
        grf_label_set_txt_color(GCL(GRF_VIEW2_ID, ROW_ID[i][COL_CHECK]), GRF_COLOR_GET(0xFF,0x9F,0x0A)); /* check = orange, both themes */
        row_badge_colors(i);
    }
}

static void theme_apply_view2(void)
{
    grf_view_set_bgcolor(GRF_VIEW2_ID, TCOL(TC_BG));   /* screen bg */
    THEME_TXT(GCL(GRF_VIEW2_ID, V2_BRAND), TC_TXT);
    THEME_TXT(GCL(GRF_VIEW2_ID, V2_SUB),   TC_TXT2);
    THEME_TXT(GCL(GRF_VIEW2_ID, V2_TITLE), TC_TXT);
    THEME_TXT(GCL(GRF_VIEW2_ID, LBL_STAT), TC_TXT2);
    THEME_TXT(GCL(GRF_VIEW2_ID, LBL_EMPTY1), TC_TXT3);
    THEME_TXT(GCL(GRF_VIEW2_ID, LBL_EMPTY2), TC_TXT3);
    use_btn_paint();   /* re-apply Use button idle colors / visibility for theme */
    view2_paint_cards(); /* row cards (selection-aware) + FIX badges */
    grf_img_set_src(GCL(GRF_VIEW2_ID, V2_NAV),
    g_dark ? "nav-profiles-light.png" : "nav-profiles.png");
}
/* ── view4 (Settings) themed control IDs ── */
#define V4_BRAND   25   /* label18 — "C-Bench"        txt   */
#define V4_SUB     24   /* label17 — "· Settings"     txt2  */
#define V4_HDR_OUT  2   /* label0  — "OUTPUT"         txt2  */
#define V4_BOOTLBL  4   /* label2  — "Boot output state" txt */
#define V4_BOOTSUB  6   /* label4  — subtitle         txt2  */
#define V4_SEP      9   /* label7  — separator line   surf2 */
#define V4_AALBL    5   /* label3  — "Auto-arm output" txt  */
#define V4_AASUB    7   /* label5  — subtitle         txt2  */
#define V4_HDR_DISP 3   /* label1  — "DISPLAY"        txt2  */
#define V4_DISPCARD 18  /* label14 — display section bg surf */
#define V4_PCT      23  /* label16 — brightness %     txt2  */
#define V4_SW       13  /* sw0     — auto-arm switch  bg=surf2 */
#define V4_IMG_SML  21  /* image1  — brightness small icon  img swap */
#define V4_IMG_FUL  22  /* image2  — brightness full icon   img swap */
#define V4_NAV      14  /* image0  — nav bar image          img swap */
#define V4_APPEAR   28  /* label21 — "Appearance"     txt   */
#define V4_APPSUB   29  /* label22 — "Dark or light"  txt2  */
#define V4_TH_DARK  33  /* label26 — "Dark"  option text    */
#define V4_TH_LIGHT 34  /* label27 — "Light" option text    */
#define V4_CH_DARK  31  /* label24 — "Dark"  chip           */
#define V4_CH_LIGHT 32  /* label25 — "Light" chip           */
#define V4_TH_SEG   30  /* label23 — theme segment bg  surf2 */
/* slider0 (V4_BRIGHT_SLD id19) track reuses TC_SURF2 */

static void theme_apply_view4(void)
{
    grf_view_set_bgcolor(GRF_VIEW4_ID, TCOL(TC_BG));                    /* screen bg */
    THEME_TXT(GCL(GRF_VIEW4_ID, V4_BRAND),    TC_TXT);
    THEME_TXT(GCL(GRF_VIEW4_ID, V4_SUB),      TC_TXT2);
    THEME_TXT(GCL(GRF_VIEW4_ID, V4_HDR_OUT),  TC_TXT2);
    THEME_TXT(GCL(GRF_VIEW4_ID, V4_BOOTLBL),  TC_TXT);
    THEME_TXT(GCL(GRF_VIEW4_ID, V4_BOOTSUB),  TC_TXT2);
    THEME_BG (GCL(GRF_VIEW4_ID, V4_SEP),      TC_SURF2);
    THEME_TXT(GCL(GRF_VIEW4_ID, V4_AALBL),    TC_TXT);
    THEME_TXT(GCL(GRF_VIEW4_ID, V4_AASUB),    TC_TXT2);
    THEME_TXT(GCL(GRF_VIEW4_ID, V4_HDR_DISP), TC_TXT2);
    THEME_BG (GCL(GRF_VIEW4_ID, V4_DISPCARD), TC_SURF);
    THEME_TXT(GCL(GRF_VIEW4_ID, V4_PCT),      TC_TXT2);
    THEME_BG (GCL(GRF_VIEW4_ID, 19), TC_SURF2);   /* slider0 id19 track (bar main part) */
    grf_ctrl_style_set_bg_color(GCL(GRF_VIEW4_ID, V4_SW), TCOL(TC_SURF2), 0); /* switch bg fill (part MAIN=0) */
        grf_img_set_src(GCL(GRF_VIEW4_ID, V4_IMG_SML),
                            g_dark ? "Brightness-symbol-small-light.png" : "Brightness-symbol-small-dark.png");
            grf_img_set_src(GCL(GRF_VIEW4_ID, V4_IMG_FUL),
                            g_dark ? "Brightness-symbol-full-light.png"  : "Brightness-symbol-full-dark.png");
            grf_img_set_src(GCL(GRF_VIEW4_ID, V4_NAV),
                            g_dark ? "nav-settings-light.png" : "nav-settings.png");
            THEME_BG(GCL(GRF_VIEW4_ID, VIEW4_LABEL6_ID),  TC_SURF);            /* major card    ID8  */
    THEME_BG(GCL(GRF_VIEW4_ID, VIEW4_LABEL10_ID), TC_SURF2);           /* segmented bg  ID12 */
    THEME_BG(GCL(GRF_VIEW4_ID, VIEW4_LABEL19_ID), TC_SURF2);           /* chip "Off"    ID26 */
    THEME_BG(GCL(GRF_VIEW4_ID, VIEW4_LABEL20_ID), TC_SURF2);           /* chip "Last"   ID27 */
        THEME_TXT(GCL(GRF_VIEW4_ID, V4_APPEAR),   TC_TXT);
        THEME_TXT(GCL(GRF_VIEW4_ID, V4_APPSUB),   TC_TXT2);
        THEME_BG (GCL(GRF_VIEW4_ID, V4_TH_SEG),   TC_SURF2);              /* theme segment bg */
            THEME_BG (GCL(GRF_VIEW4_ID, V4_CH_DARK),  TC_SURF2);              /* theme chips */
            THEME_BG (GCL(GRF_VIEW4_ID, V4_CH_LIGHT), TC_SURF2);
        boot_state_paint(g_v4_boot);   /* text colors (TC_TXT/grey) + chip show-hide */
        theme_state_paint();           /* Dark/Light texts + chip show-hide */
    }

static void theme_apply(void)               /* repaint all themed views from g_dark */
{
	theme_apply_view1();
	    theme_apply_view2();
	    theme_apply_view4();
	}
	void view1_apply_theme(void) { theme_apply(); }   /* view1 entry: repaint from shadow */
	void view2_apply_theme(void) { theme_apply(); }   /* view2 entry: repaint from shadow */
	void view4_apply_theme(void) { theme_apply(); }   /* view4 entry: repaint from shadow */
void view1_toggle_theme(void)               /* user tap: flip + apply + persist */
{
    g_dark ^= 1;
    theme_apply();
    grf_reg_set(0x0039, g_dark);
    grf_reg_com_send(0x0039, 1);
}

void view2_reset_panel(void)
{
    grf_label_set_txt(GCL(GRF_VIEW2_ID, ADJ_LV), "0.00 V");
    grf_label_set_txt(GCL(GRF_VIEW2_ID, ADJ_LC), "0.0 A");
    grf_ctrl_set_hidden(GCL(GRF_VIEW2_ID, ADJ_CONT), 1);
    grf_ctrl_set_hidden(GCL(GRF_VIEW2_ID, SEL_BOX), 1); /* hide selection border until a row is picked */
    g_panel_up = 0;
    grf_ctrl_set_ext_click_area(GCL(GRF_VIEW2_ID, ADJ_SV), 24); /* enlarge hit area, keep graphics */
    grf_ctrl_set_ext_click_area(GCL(GRF_VIEW2_ID, ADJ_SC), 24);
    use_btn_set(0, "Select a rail");
    grf_reg_set(0x0024, 1); /* ask the RP to (re)push the PDO list now */
    grf_reg_com_send(0x0024, 1);
}

/* ---- view4 (Settings) ---- */
static u8 g_v4_autoarm = 0; /* shadow of reg 0x0032 (0/1)                */

#define V4_BRIGHT_SLD 19      /* slider0 - brightness 10..100 % */
#define V4_BRIGHT_LBL 23      /* label16 - brightness %         */
static u8 g_v4_bright = 100;  /* shadow of reg 0x0030 brightness % (10..100) */
static u8 g_bright_guard = 0; /* suppress slider VALUE_CHANGED echo on programmatic set */

static void bright_label(u8 pct)
{
    char b[8];
    snprintf(b, sizeof(b), "%u%%", pct);
    grf_label_set_txt(GCL(GRF_VIEW4_ID, V4_BRIGHT_LBL), b);
}
static void bright_backlight(u8 pct) { grf_disp_set_bright((u8)((u16)pct * 99 / 100)); } /* 10..100% -> 0..99 */
static void bright_slider(u8 pct)
{
    g_bright_guard = 1;
    grf_slider_set_value(GCL(GRF_VIEW4_ID, V4_BRIGHT_SLD), pct);
    g_bright_guard = 0;
}

static u32 g_sess_mWh = 0; /* reassembled 32-bit session energy (mWh) */
static void wh_paint(void) /* show as "X.XXX" (mWh resolution) */
{
    char b[16];
    snprintf(b, sizeof(b), "%u.%03u", (unsigned)(g_sess_mWh / 1000), (unsigned)(g_sess_mWh % 1000));
    grf_label_set_txt(GCL(GRF_VIEW1_ID, LBL_WH), b);
}

#define LBL_ELAPSED 26           /* view1 session elapsed (VIEW1_LABEL22_ID) */
static void elapsed_paint(u16 s) /* MM:SS, rolls to H:MM:SS past 1h */
{
    char b[12];
    if (s < 3600)
        snprintf(b, sizeof(b), "%u:%02u", s / 60, s % 60);
    else
        snprintf(b, sizeof(b), "%u:%02u:%02u", s / 3600, (s % 3600) / 60, s % 60);
    grf_label_set_txt(GCL(GRF_VIEW1_ID, LBL_ELAPSED), b);
}

static void boot_state_paint(u8 last_used) /* 0 = Off white, 1 = Last used white */
{
    grf_color_t on  = TCOL(TC_TXT);                                                    /* selected   = primary */
    grf_color_t off = GRF_COLOR_GET(0x98, 0x98, 0x9F);                                 /* unselected = grey    */
    grf_label_set_txt_color(GCL(GRF_VIEW4_ID, VIEW4_LABEL8_ID), last_used ? off : on); /* "Off"       ID10 */
    grf_label_set_txt_color(GCL(GRF_VIEW4_ID, VIEW4_LABEL9_ID), last_used ? on : off); /* "Last used" ID11 */
    /* selection chip: show behind the active option, hide the other */
    grf_ctrl_set_hidden(GCL(GRF_VIEW4_ID, VIEW4_LABEL19_ID), last_used ? 1 : 0);       /* chip "Off"       ID26 */
    grf_ctrl_set_hidden(GCL(GRF_VIEW4_ID, VIEW4_LABEL20_ID), last_used ? 0 : 1);       /* chip "Last used" ID27 */
}

static void theme_state_paint(void) /* g_dark: 0=Dark selected, 1=Light selected */
{
    grf_color_t on  = TCOL(TC_TXT);                     /* selected   = primary */
    grf_color_t off = GRF_COLOR_GET(0x98, 0x98, 0x9F);  /* unselected = grey    */
    grf_label_set_txt_color(GCL(GRF_VIEW4_ID, V4_TH_DARK),  g_dark ? off : on); /* "Dark"  ID33 */
    grf_label_set_txt_color(GCL(GRF_VIEW4_ID, V4_TH_LIGHT), g_dark ? on : off); /* "Light" ID34 */
    grf_ctrl_set_hidden(GCL(GRF_VIEW4_ID, V4_CH_DARK),  g_dark ? 1 : 0);        /* chip Dark  ID31 */
    grf_ctrl_set_hidden(GCL(GRF_VIEW4_ID, V4_CH_LIGHT), g_dark ? 0 : 1);        /* chip Light ID32 */
}

void view4_set_theme(u8 light) /* user tap: set absolute theme + apply + persist */
{
    if (g_dark == light)
        return;                 /* already in this mode */
    g_dark = light ? 1 : 0;
    theme_apply();              /* repaints all views incl. theme_state_paint */
    grf_reg_set(0x0039, g_dark);
    grf_reg_com_send(0x0039, 1);
}

void view4_set_boot_state(u8 last_used) /* user tap: paint + send */
{
    g_v4_boot = last_used;
    boot_state_paint(last_used);
    grf_reg_set(0x0031, last_used);
    grf_reg_com_send(0x0031, 1);
}

void view4_apply_settings(void) /* entry: paint controls from shadow */
{
    boot_state_paint(g_v4_boot);
    grf_sw_set_state(GCL(GRF_VIEW4_ID, VIEW4_SW0_ID), g_v4_autoarm);
    grf_slider_set_range(GCL(GRF_VIEW4_ID, V4_BRIGHT_SLD), 10, 100);
    bright_slider(g_v4_bright);
    bright_label(g_v4_bright);
}

void view4_set_autoarm(u8 on) /* 0/1 */
{
    g_v4_autoarm = on;
    grf_reg_set(0x0032, on);
    grf_reg_com_send(0x0032, 1);
}

void view4_set_bright(u8 pct) /* slider moved: live backlight + label + persist on RP */
{
    if (g_bright_guard)
        return; /* programmatic set -> don't echo back */
    if (pct < 10)
        pct = 10;
    else if (pct > 100)
        pct = 100;
    g_v4_bright = pct;
    bright_backlight(pct); /* instant local dimming */
    bright_label(pct);
    grf_reg_set(0x0030, pct);
    grf_reg_com_send(0x0030, 1);
}

void view4_request_settings(void) /* ask RP to push stored 0x0031/0x0032 back */
{
    grf_reg_set(0x0033, 1);
    grf_reg_com_send(0x0033, 1);
}

static void fill_row(u8 i, prof_t *p)
{
    char v[20], c[16];
    const char *badge;
    grf_color_t bbg, btx;
    switch (p->type)
    {
    case 1:
        badge = "PPS";
        bbg = GRF_COLOR_GET(0x64, 0xD2, 0xFF);
        btx = GRF_COLOR_GET(0x06, 0x2A, 0x30);
        break;
    case 2:
        badge = "AVS";
        bbg = GRF_COLOR_GET(0xFF, 0x9F, 0x0A);
        btx = GRF_COLOR_GET(0x2A, 0x18, 0x00);
        break;
    case 3:
        badge = "EPR";
        bbg = GRF_COLOR_GET(0xBF, 0x5A, 0xF2);
        btx = GRF_COLOR_GET(0x1E, 0x0C, 0x33);
        break;
    default:
            badge = "FIX";
            bbg = TCOL(TC_CHIP);
            btx = TCOL(TC_TXT2);
        break;
    }
    u8 range = (p->vmin != p->vmax);

    if (!range)
        snprintf(v, sizeof(v), "%u.%02u V", p->vmin / 1000, (p->vmin % 1000) / 10);
    else
        snprintf(v, sizeof(v), "%u.%u-%u.%u V", p->vmin / 1000, (p->vmin % 1000) / 100,
                 p->vmax / 1000, (p->vmax % 1000) / 100);
    snprintf(c, sizeof(c), "%u.%u A", p->imax / 1000, (p->imax % 1000) / 100);

    grf_label_set_txt(GCL(GRF_VIEW2_ID, ROW_ID[i][COL_BADGE]), badge);
    grf_ctrl_style_set_bg_color(GCL(GRF_VIEW2_ID, ROW_ID[i][COL_BADGE]), bbg, 0);
    grf_label_set_txt_color(GCL(GRF_VIEW2_ID, ROW_ID[i][COL_BADGE]), btx);
    grf_label_set_txt(GCL(GRF_VIEW2_ID, ROW_ID[i][COL_VOLT]), v);
    grf_label_set_txt(GCL(GRF_VIEW2_ID, ROW_ID[i][COL_META]), range ? "Adjustable rail" : "Fixed rail");
    grf_label_set_txt(GCL(GRF_VIEW2_ID, ROW_ID[i][COL_CURR]), c);
    grf_label_set_txt(GCL(GRF_VIEW2_ID, ROW_ID[i][COL_CHECK]), "\xE2\x9C\x93"); /* tick, overrides placeholder */
    grf_label_set_txt_color(GCL(GRF_VIEW2_ID, ROW_ID[i][COL_CHECK]), GRF_COLOR_GET(0xFF, 0x9F, 0x0A));
    show_row(i, 1);
}

void view2_apply_status(void) /* label91: source summary or empty-state prompt */
{
    if (g_prof_n)
    {
        u32 maxmw = 0;
        for (u8 i = 0; i < g_prof_n; i++)
        {
            u32 mw = (u32)g_prof[i].vmax * g_prof[i].imax / 1000; /* mV*mA/1000 = mW */
            if (mw > maxmw) maxmw = mw;
        }
        u16 w = (u16)((maxmw + 500) / 1000);
        char sb[48];
        snprintf(sb, sizeof(sb), "%u W USB-C \xC2\xB7 %u profile%s",
                 w, g_prof_n, (g_prof_n == 1) ? "" : "s");
        grf_label_set_txt(GCL(GRF_VIEW2_ID, LBL_STAT), sb);
    }
    else
    {
        grf_label_set_txt(GCL(GRF_VIEW2_ID, LBL_STAT),
            "Connect a USB-C source to the other type-C port to see its power profiles");
    }
}

void grf_reg_set_user(u16 addr, u16 *data, u8 datalen)
{
    char buf[16];

    /* profile rows: 0x0110 + i*4 = [type, vmin, vmax, imax] */
    if (addr >= 0x0110 && addr < 0x0110 + MAX_PROF * 4 && datalen >= 4)
    {
        u8 i = (addr - 0x0110) / 4;
        g_prof[i].type = data[0];
        g_prof[i].vmin = data[1];
        g_prof[i].vmax = data[2];
        g_prof[i].imax = data[3];
        return;
    }

    switch (addr)
    {
    case 0x0010: /* voltage mV (numeric label only; arc eased via 0x001B) */
            snprintf(buf, sizeof(buf), "%u.%02u", data[0] / 1000, (data[0] % 1000) / 10);
            grf_label_set_txt(GCL(GRF_VIEW1_ID, LBL_VOLT), buf);
            break;
    case 0x001B: /* eased arc value (0..280) from RP analog ramp */
            g_arc = data[0];
            grf_arc_set_value(GCL(GRF_VIEW1_ID, ARC_VOLT), g_arc);
            break;
    case 0x0011: /* current mA */
        snprintf(buf, sizeof(buf), "%u.%03u A", data[0] / 1000, data[0] % 1000);
        grf_label_set_txt(GCL(GRF_VIEW1_ID, LBL_CURR), buf);
        break;
    case 0x0012: /* power dW (0.1W) */
        snprintf(buf, sizeof(buf), "%u.%u W", data[0] / 10, data[0] % 10);
        grf_label_set_txt(GCL(GRF_VIEW1_ID, LBL_POWER), buf);
        break;
    case 0x0013: /* session energy mWh, low 16 */
        g_sess_mWh = (g_sess_mWh & 0xFFFF0000UL) | (u32)data[0];
        wh_paint();
        break;
    case 0x0014: /* session energy mWh, high 16 */
        g_sess_mWh = (g_sess_mWh & 0x0000FFFFUL) | ((u32)data[0] << 16);
        wh_paint();
        break;
    case 0x0018: /* session elapsed seconds */
        elapsed_paint(data[0]);
        break;
    case 0x0019: /* active profile type */
        g_ap_type = (u8)data[0];
        ap_paint();
        break;
    case 0x001A: /* active profile setpoint mV */
        g_ap_mV = data[0];
        ap_paint();
        break;
    case 0x0016: /* real output state from RP -> drive toggle */
        g_out_on = data[0];
        view1_set_output_btn(g_out_on);
        break;
    case 0x0017: /* active rail position from RP -> highlight on list render */
        g_applied = (data[0] >= MAX_PROF) ? 0xFF : (u8)data[0];
        break;
    case 0x0031: /* boot output state from RP -> shadow + live repaint if on view4 */
        g_v4_boot = data[0];
        if (grf_view_get_cur_id(GRF_LAYER_UI) == GRF_VIEW4_ID)
            boot_state_paint(g_v4_boot);
        break;
    case 0x0030: /* brightness % from RP -> backlight now; repaint if on view4 */
        g_v4_bright = (data[0] < 10) ? 10 : (data[0] > 100 ? 100 : data[0]);
        bright_backlight(g_v4_bright); /* global: applies even when off view4 */
        if (grf_view_get_cur_id(GRF_LAYER_UI) == GRF_VIEW4_ID)
        {
            bright_slider(g_v4_bright);
            bright_label(g_v4_bright);
        }
        break;
    case 0x0032: /* auto-arm from RP -> shadow + live repaint if on view4 */
        g_v4_autoarm = data[0];
        if (grf_view_get_cur_id(GRF_LAYER_UI) == GRF_VIEW4_ID)
            grf_sw_set_state(GCL(GRF_VIEW4_ID, VIEW4_SW0_ID), g_v4_autoarm);
        break;
    case 0x0039:  /* theme from RP -> shadow + apply */
         g_dark = data[0] ? 1 : 0;
         theme_apply();
        break;
    case 0x0101:
    { /* list ready -> render */
        g_prof_n = grf_reg_get(0x0100);
        if (g_prof_n > MAX_PROF)
            g_prof_n = MAX_PROF;
        g_sel = 0xFF;
        grf_ctrl_set_hidden(GCL(GRF_VIEW2_ID, ADJ_CONT), 1);
        use_btn_set(0, "Select a rail");
        grf_ctrl_set_hidden(GCL(GRF_VIEW2_ID, LBL_EMPTY1), g_prof_n ? 1 : 0);
        grf_ctrl_set_hidden(GCL(GRF_VIEW2_ID, LBL_EMPTY2), g_prof_n ? 1 : 0);
        for (u8 i = 0; i < g_prof_n; i++)
        {
            g_prof[i].type = grf_reg_get(0x0110 + i * 4 + 0);
            g_prof[i].vmin = grf_reg_get(0x0110 + i * 4 + 1);
            g_prof[i].vmax = grf_reg_get(0x0110 + i * 4 + 2);
            g_prof[i].imax = grf_reg_get(0x0110 + i * 4 + 3);
            fill_row(i, &g_prof[i]);
        }
        for (u8 i = g_prof_n; i < MAX_PROF; i++)
                    show_row(i, 0); /* hide unused rows */
                view2_apply_status(); /* refresh label91 from the new list */
        for (u8 i = 0; i < g_prof_n; i++)
            grf_ctrl_set_hidden(GCL(GRF_VIEW2_ID, ROW_ID[i][COL_CHECK]), 1);
        if (g_prof_n == 0)
            g_applied = 0xFF; /* source gone -> forget selection */
        if (g_applied != 0xFF && g_applied < g_prof_n)
                {
                    g_sel = g_applied; /* show active rail */
                    highlight_row(g_applied, 1);
                }
                view2_paint_cards(); /* apply all row column colors after render */
                break;
    }
    }
}

#define HEAD_FH 0x5A
#define HEAD_FL 0xA5
#define REG_LEN 0x800
static u16 ctrlreg[REG_LEN] = {0};

s32 grf_reg_set(u16 addr, u16 data)
{
    if (addr > REG_LEN)
    {
        return GRF_FAIL;
    }
    ctrlreg[addr] = data;
    // grf_printf("set reg %04X = %04X\n",addr,data);
    return GRF_OK;
}

s32 grf_reg_get(u16 addr)
{
    if (addr > REG_LEN)
    {
        return GRF_FAIL;
    }
    // grf_printf("get reg %04X = %04X\n",addr,ctrlreg[addr]);
    return ctrlreg[addr];
}
//*********write reg**********
// TX - FH FL len cmd addr data0 data1
//     5A A5 07  82  0001 FFFF  FFFF
// RX - 无

//*********read reg**********
// TX -  FH FL len cmd addr reglen
//      5A A5 04  83  0001  02
// RX -  FH FL len cmd addr reglen data0 data1
//      5A A5 08  83  0001  02    FFFF  FFFF

static s32 grf_reg_s_set(u16 addr, u8 *data, u8 len)
{
    if (addr > REG_LEN)
    {
        return GRF_FAIL;
    }
    u8 i = 0;
    for (i = 0; i < len; i++)
    {
        ctrlreg[addr + i] = (data[i * 2] << 8) + data[i * 2 + 1];
        // grf_printf("reg_s_set %04X = %04X\n",addr+i,ctrlreg[addr+i]);
        grf_reg_set_user(addr, ctrlreg + addr + i, 1);
    }
    return GRF_OK;
}
s32 grf_reg_com_send(u16 addr, u16 len)
{
    u8 txdata[257] = {0};
    u32 i = 0, j = 0;
    txdata[i++] = HEAD_FH;
    txdata[i++] = HEAD_FL;
    txdata[i++] = len * 2 + 4;
    txdata[i++] = 0x83;
    txdata[i++] = addr >> 8;
    txdata[i++] = (addr & 0x00ff);
    txdata[i++] = len;
    for (j = 0; j < len; j++)
    {
        txdata[i++] = (ctrlreg[addr + j] >> 8);
        txdata[i++] = (ctrlreg[addr + j] & 0x00ff);
    }
    grf_drv_uart_send(drv_uart, txdata, i);
}

static s32 grf_comm_handle(u8 *data)
{
    u8 cmd = data[3];
    u8 len = data[2];
    u16 addr = (data[4] << 8) + data[5];
    u32 i = 0;
    if (addr > REG_LEN)
    {
        return GRF_FAIL;
    }
    //    grf_printf("cmd=%x\n",cmd);
    switch (cmd)
    {
    case 0x82: // 写寄存器
    {
        u32 regcount = (len - 3) >> 1;
        grf_reg_s_set(addr, data + 6, regcount);
    }
    break;
    case 0x83: // 读寄存器
        grf_reg_com_send(addr, data[6]);
        break;
    }
    return GRF_OK;
}

#define UART_LASTBUFF 1
#if UART_LASTBUFF
#define RX_BUF_LEN 1024
static u8 RX_HAND_BUF[RX_BUF_LEN];
#endif
static void recive_data_handle(u8 *databuf, u32 datalen)
{
    u16 i = 0;
    static u16 last_data_num = 0;
#if UART_LASTBUFF
    if (last_data_num + datalen > RX_BUF_LEN)
    {
        datalen = RX_BUF_LEN - last_data_num;
    }
    if (last_data_num)
    {
        memcpy(RX_HAND_BUF + last_data_num, databuf, datalen);
        databuf = RX_HAND_BUF;
        datalen += last_data_num;
        last_data_num = 0;
    }
#endif
#if 0
    u16 j;
    grf_printf("uart rx[%d]:",datalen);
	for(j = 0;j < datalen; j++){
		grf_printf(" %02X",databuf[j]);
	}
	grf_printf("\n",datalen);
#endif
    if (datalen >= 6)
    { // 最短的指令为6个
        for (i = 0; i <= datalen - 6; i++)
        {
            if ((databuf[i] == HEAD_FH) && (databuf[i + 1] == HEAD_FL))
            {
                if (databuf[i + 2] <= (datalen - i - 3))
                {
                    if (grf_comm_handle(databuf + i) == GRF_OK)
                    {
                        i += (databuf[i + 2] + 3) - 1;
                    }
                }
            }
        }
    }
    if (i < datalen)
    {
        last_data_num = datalen - i;
    }

#if UART_LASTBUFF
    if (last_data_num <= 256 && last_data_num != 0)
    {
        u8 last_data_buf[256] = {0};
        memcpy(last_data_buf, databuf + i, last_data_num);
        memcpy(RX_HAND_BUF, last_data_buf, last_data_num);
    }
    else
    {
        last_data_num = 0;
    }
#else
    last_data_num = 0;
#endif
}

void grf_uart_init(void)
{
    grf_uart_cfg_t cfg_t = {0};
    cfg_t.port = 6;
    cfg_t.win_port = -1;
    cfg_t.speed_e = UART_SPEED_115200;
    cfg_t.bit_e = UART_BIT_8;
    cfg_t.parity_e = UART_PARITY_NONE;
    cfg_t.stop_e = UART_STOP_1;
    drv_uart = grf_drv_uart_open(cfg_t);
    if (drv_uart)
    {
        grf_drv_uart_rev_set_bfun(drv_uart, recive_data_handle, 1024);
    }
}

#endif
