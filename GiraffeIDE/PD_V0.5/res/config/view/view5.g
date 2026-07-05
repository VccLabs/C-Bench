{
  "CLASSNAME" : "view",
  "controls" : [ {
    "CLASSNAME" : "txtbox",
    "theme" : "youth",
    "type" : 12,
    "property" : {
      "pub_t" : {
        "name" : "txtbox0",
        "id" : 1,
        "ctrl_type" : 12,
        "pt" : {
          "x" : 95,
          "y" : 300
        },
        "size" : {
          "width" : 476,
          "height" : 101
        },
        "t_mode" : 1,
        "scrollbar_t" : {
          "dir_e" : 15
        },
        "alpha" : 255,
        "pub" : true
      },
      "line_mode" : 1,
      "txt_align_e" : 1,
      "max_txt_len" : -1,
      "p_holder_txt" : "TxtBox",
      "cursor_style_t" : {
        "cursor_e" : 4,
        "grf_cursor_width" : 1,
        "grf_cursor_opa" : 255,
        "grf_cursor_animtime" : 500
      }
    }
  }, {
    "CLASSNAME" : "keyboard",
    "theme" : "youth",
    "type" : 23,
    "property" : {
      "pub_t" : {
        "name" : "keyboard0",
        "id" : 2,
        "ctrl_type" : 5,
        "pt" : {
          "y" : 464
        },
        "size" : {
          "width" : 720,
          "height" : 256
        },
        "t_mode" : 1,
        "scrollbar_t" : { },
        "alpha" : 255,
        "pub" : true
      },
      "mode" : 2,
      "popovers" : true,
      "cn_font_size" : 30,
      "styles" : [ {
        "textFlag" : true,
        "name" : "bg_style_t",
        "img_t" : { }
      }, {
        "textFlag" : true,
        "style_part" : 327680,
        "name" : "btn_style_re",
        "img_t" : { }
      }, {
        "textFlag" : true,
        "style_part" : 327680,
        "style_state" : 32,
        "name" : "btn_style_pr",
        "img_t" : { }
      } ]
    },
    "modelType" : "keyboard"
  } ],
  "view" : {
    "v_name" : "view5",
    "v_id" : 5,
    "bg_scale" : 255,
    "bg_color" : 4294967295,
    "bg_image" : "view5.png"
  }
}