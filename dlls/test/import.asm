; macro to define variables
%macro var 2
	global _%1
	_%1: equ %2
%endmacro

; macro to define functions
%macro func 2
	global _%1
	_%1:
	jmp[.a]
	.a: dd %2
%endmacro

; macro to define c++ function jumpers
%macro funp 2
	global _%1
	_%1: dd %2
%endmacro

; ---------------------------------------
; - VARIABLES							-
; ---------------------------------------
section .data

var Random,				0x51F19C

var pBuffer,			0xA3A08C
var dwQuit_game,		0xA3A094
var G,					0xA5FB80
var Moji,				0xA552A0
var byte_A44F30,		0xA44F30
var byte_A5DB8B,		0xA5DB8B
var byte_A5DB8C,		0xA5DB8C
var pGfxPool,			0xA61C0C
var GsEnv,				0xA67A80
var pGsEnv,				0xA67CC8
var VSync_rate,			0xA67CD4
var Bg_chg_flg,			0xA67CD6

var transmissionBuffer,	0xA523EC
var MovieId,			0xA5516C

var St_mode,			0xA3B204
var Inverse_flg,		0xA5DB88
var byte_A5DBC0,		0xA5DBC0
var Game,				0xA5DBC1

var byte_A67CE4,		0xA67CE4

; movie stuff
var MovEnv,				0xA55128
var unk_A67AE0,			0xA67AE0
var word_A5515C,		0xA5515C
var Movie_r0,			0xA55154
var Movie_r1,			0xA55156
var Movie_r2,			0xA55158
var Movie_r3,			0xA55159
var Movie_r4,			0xA5515A
var Movie_r5,			0xA5515B
var Movie_stop_bk,		0xA55168
var byte_A551FA,		0xA551FA
var byte_A5516E,		0xA5516E

var byte_A67CD5,		0xA67CD5

var movie_path,			0x5397D0

var Is_loading,			0xA67CDB

var File_size_tbl,		0x534D28
var File_page_tbl,		0x534F5C

var Opt,				0xA465A0
var Option_main_sel,	0xA465A4
var Option_snd_sel,		0xA465A6
var Option_gfx_sel,		0xA465AA
var Option_hue,			0xA465B0
var Option_pool,		0xA47A6C
var Option_wtbl,		0x52B31C

var Em_lmatrix,			0x51F740
var Em_pcode,			0x51F746
var Em_rgb,				0x51F748
var chrome,				0x51F760

var Camera_m,			0x51F818
var pCurScale,			0xA50BF0
var pCurM,				0xA50BF4
var word_A50C00,		0xA50C00

var Camera_bk,			0xA523A8

; water stuff
var byte_5347AC,		0x5347AC
var byte_5347E0,		0x5347E0
var word_5347FC,		0x5347FC

var byte_52DEA0,		0x52DEA0
var word_52DEC8,		0x52DEC8

; sound stuff
var vol_bgm_tbl,		0x51A7C8
var vol_sbgm0_tbl,		0x51A810
var vol_sbgm1_tbl,		0x51A858
var bgm_vol_lut,		0x51A8A0
var seq_scores,			0xA5E78C
var key_data,			0xA5EBA0
var Sound_id,			0xA5F255
var Vol_sfx,			0xA5EEA8
var Vol_bgm,			0xA5EEAA
var Vol_sfx_old,		0xA5EEB0
var Vol_bgm_old,		0xA5EEB2

var main_bgm_data,		0x518AE0

var Title_env,			0xA449E0
var flash_tbl,			0x51F2C4

var Time_dump,			0xA449D4

; esp stuff
var Esp_pool,			0xA3CF60
var pEsp_pool,			0xA44760

; player stuff
var pl_mv_tbl,			0x52D538
var wp_turn_speed,		0x52D6FB
var item_data,			0x52DF78
var byte_A3A7E5,		0xA3A7E5
var stru_A3B200,		0xA3B200
var Key_id,				0xA67CDD

%include "import_d.asm"

; DEFINITIONS


; ---------------------------------------
; - FUNCTIONS							-
; ---------------------------------------
section .text

%include "import_f.asm"

func SetGeomOffset,	0x50A5D0
func SetTile,		0x425670

func OpenTIM,		0x4254B0
func ReadTIM,		0x4254D0

func ClearOTagR,	0x425530
;func Display,		0x425C90
func InverseBG,		0x4263C0
func DisplayBlack,	0x427270
func Dec_sld,		0x427640
func MergePrim,		0x4276F7
func Flg_on,		0x428D90
func Flg_off,		0x428DB0
func Flg_ck,		0x428DE0
func Rand,			0x428E00
func Upload_tim,	0x428980
func Bgm_update,	0x429A90
func Fade_set,		0x429E60
func Snd_se_on,		0x429E60
func Load_room_bg,	0x426A90
func Printf,		0x42F810
func Init_system,	0x426480
func QuitGame,		0x426860
func Moji_trans,	0x4269D0
func Reboot,		0x426E60
func Disp_pause,	0x426F00
func re3_strcat,	0x427700
func Hit_ck_pos4,	0x428AC0
func Set_camera,	0x428E40
func Set_light_data,0x428FC0
func Sound_kill,	0x42BFC0
func Pad_read,		0x42F020
func Moji_42F7F0,	0x42F7F0
func Disp_ascii16,	0x42F840
func Mess_set,		0x430020
func Get_item_string,0x431A10
func Moji_init,		0x431BB0
func Set_room,		0x46B740
func door_scheduler,0x46CA70
func Door_trans,	0x46CB10
func Rnd,			0x428E00
func Set_item_img,	0x4DFFE0
func Search_item,	0x4E1CE0
func Dec_item,		0x4E1D20
func op_new,		0x4F1853
func gte_ldsv012_0,	0x50AF00
func gte_ldv0,		0x50AEE0
func gte_ncs,		0x50B0B0
func gte_ncss,		0x50B0A0
func LoadAverageShort0,	0x50AA00

func CompM,			0x427580
func SetRotMatrix,	0x50A6F0
func SetTransMatrix,0x50A7D0
func gte_stmac,		0x50AF40
func scale_vec,		0x50AFA0
func gte_rt,		0x50B0E0
func SetGeomScreen,	0x50A5F0
func SetLightMatrix,0x50A880
func StoreLMatrix,	0x50A8A0
func ApplyRotMatrix,0x50A8C0

func trans_colors,	0x436CC0
func avg_normals0,	0x437090
func avg_normals1,	0x436E70
func override_otz,	0x45DFC0

;func Fade_set,		0x429E60
func Fade_adjust,	0x426FB0
func Fade_reset,	0x427230
func Fade_status,	0x427250

func Bg_set_mode,	0x426F70

func Disp_mess,		0x431650

func Title_boot,	0x453EA0
func Mercs_moji,	0x453610

func hash_to_value,	0x406600
func value_to_hash,	0x406810

; movie stuff
func Movie_set,		0x431D80
func Movie_0,		0x431FC0
func Movie_1,		0x432210
func Movie_2,		0x432410
func movie_status,	0x432690
func movie_can_swap,0x50A4D0

; option stuff
func Option_disp_bg,	0x44CC00
func Option_arrow,		0x44DF20
func Option_num,		0x44DE20

func SsExLoadBgm,			0x427890
func Snd_mute,				0x429DF0
func Snd_load,				0x42C8F0
func Snd_transfer_buffer,	0x42CE30

funp re3snd__ctor,	0x4146F0
funp re3input__ctor,0x408C20

; music stuff
func SsExSeqStop,		0x427C80
func SsExSeqSetVol0,	0x427DA0
func SsExDetectRoom,	0x4281D0

func SsSeqReleasing,	0x50A4E0
func SsSeqStart,		0x50A500
func SsSeqSetLoop,		0x50A520
func SsSeqRelease,		0x50A540
func SsSeqHalt,			0x50A560
func SsSeqSetVol,		0x50A580
func SsSeqGetState,		0x50A5A0

func SsExSeqSetVol4,	0x4296F0

; player stuff
func Joint_move,		0x438D50
func Tama_ck_i,			0x4E1E70
