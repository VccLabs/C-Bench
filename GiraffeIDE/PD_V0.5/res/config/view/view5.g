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
          "x" : 25,
          "y" : 91
        },
        "size" : {
          "width" : 670,
          "height" : 158
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
      "p_holder_txt" : "Type your message here ..",
      "init_txt" : "This is your gift message, shown on boot. Edit or turn it off anytime in Settings > Gift Message.\r\nLearn more at: https://vcclabs.com",
      "cursor_style_t" : {
        "cursor_e" : 4,
        "color" : 4278879487,
        "grf_cursor_width" : 2,
        "grf_cursor_opa" : 255,
        "grf_cursor_animtime" : 400
      },
      "styles" : [ {
        "name" : "bg_style_t",
        "body_t" : {
          "main_color" : 1842206,
          "radius" : 18,
          "border" : {
            "color" : 689407
          },
          "padding" : {
            "top" : 20,
            "left" : 25,
            "right" : 25
          }
        },
        "txt_t" : {
          "color" : 16777215,
          "font_size" : 20
        },
        "img_t" : { }
      }, {
        "style_part" : 65536,
        "name" : "sb_style_t",
        "body_t" : {
          "main_color" : 16776960,
          "radius" : 2,
          "border" : {
            "color" : 5197647
          },
          "padding" : {
            "top" : 2147483647,
            "bottom" : 2147483647,
            "left" : 2147483647,
            "right" : 2147483647,
            "inner" : 2147483647
          }
        },
        "txt_t" : {
          "color" : 16711680,
          "opa" : 2147483647
        },
        "img_t" : { }
      }, {
        "style_part" : 524288,
        "name" : "p_holder_style_t",
        "txt_t" : {
          "color" : 5197647
        },
        "img_t" : { }
      } ]
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
  }, {
    "CLASSNAME" : "label",
    "theme" : "youth",
    "property" : {
      "pub_t" : {
        "name" : "label0",
        "id" : 3,
        "pt" : {
          "x" : 31,
          "y" : 261
        },
        "size" : {
          "width" : 210,
          "height" : 16
        },
        "scrollbar_t" : { },
        "alpha" : 255,
        "pub" : true
      },
      "animspeed" : 50,
      "align_e" : 33,
      "txt" : "Shown on boot when enabled",
      "styles" : [ {
        "name" : "style",
        "body_t" : {
          "padding" : {
            "top" : 2147483647,
            "bottom" : 2147483647,
            "left" : 2147483647,
            "right" : 2147483647,
            "inner" : 2147483647
          }
        },
        "txt_t" : {
          "color" : 3552822,
          "font_size" : 14
        },
        "img_t" : { }
      } ]
    }
  }, {
    "CLASSNAME" : "label",
    "theme" : "youth",
    "property" : {
      "pub_t" : {
        "name" : "label1",
        "id" : 4,
        "pt" : {
          "x" : 30,
          "y" : 68
        },
        "size" : {
          "width" : 210,
          "height" : 16
        },
        "scrollbar_t" : { },
        "alpha" : 255,
        "pub" : true,
        "controlIDEnum" : "VIEW5_LABEL1_ID"
      },
      "animspeed" : 50,
      "align_e" : 33,
      "txt" : "YOUR MESSAGE",
      "styles" : [ {
        "name" : "style",
        "body_t" : {
          "padding" : {
            "top" : 2147483647,
            "bottom" : 2147483647,
            "left" : 2147483647,
            "right" : 2147483647,
            "inner" : 2147483647
          }
        },
        "txt_t" : {
          "color" : 9276819,
          "font_size" : 14
        },
        "img_t" : { }
      } ]
    }
  }, {
    "CLASSNAME" : "label",
    "theme" : "youth",
    "property" : {
      "pub_t" : {
        "name" : "label2",
        "id" : 5,
        "pt" : {
          "x" : 25,
          "y" : 290
        },
        "size" : {
          "width" : 670,
          "height" : 66
        },
        "t_mode" : 1,
        "scrollbar_t" : { },
        "alpha" : 255,
        "pub" : true
      },
      "auto_hight" : 1,
      "animspeed" : 50,
      "align_e" : 34,
      "txt" : "Save",
      "styles" : [ {
        "name" : "style",
        "body_t" : {
          "main_color" : 689407,
          "radius" : 18,
          "opa" : 255,
          "padding" : {
            "top" : 2147483647,
            "bottom" : 2147483647,
            "left" : 2147483647,
            "right" : 2147483647,
            "inner" : 2147483647
          }
        },
        "txt_t" : {
          "color" : 16777215,
          "font_lib" : "Inter_24pt-ExtraBold.ttf",
          "font_size" : 24
        },
        "img_t" : { }
      } ]
    }
  }, {
    "CLASSNAME" : "label",
    "theme" : "youth",
    "property" : {
      "pub_t" : {
        "name" : "label3",
        "id" : 6,
        "pt" : { },
        "size" : {
          "width" : 121,
          "height" : 66
        },
        "scrollbar_t" : { },
        "alpha" : 255,
        "pub" : true,
        "controlIDEnum" : "VIEW5_LABEL3_ID"
      },
      "auto_hight" : 1,
      "animspeed" : 50,
      "align_e" : 34,
      "txt" : "‹  Settings",
      "styles" : [ {
        "name" : "style",
        "body_t" : {
          "padding" : {
            "top" : 2147483647,
            "bottom" : 1,
            "left" : 10,
            "right" : -1,
            "inner" : 2147483647
          }
        },
        "txt_t" : {
          "color" : 689407,
          "font_lib" : "Inter_24pt-ExtraBold.ttf",
          "font_size" : 17
        },
        "img_t" : { }
      } ]
    }
  }, {
    "CLASSNAME" : "label",
    "theme" : "youth",
    "property" : {
      "pub_t" : {
        "name" : "label4",
        "id" : 7,
        "pt" : {
          "x" : 277
        },
        "size" : {
          "width" : 156,
          "height" : 66
        },
        "scrollbar_t" : { },
        "alpha" : 255,
        "pub" : true,
        "controlIDEnum" : "VIEW5_LABEL4_ID"
      },
      "auto_hight" : 1,
      "animspeed" : 50,
      "align_e" : 34,
      "txt" : "Gift Mesage",
      "styles" : [ {
        "name" : "style",
        "body_t" : {
          "padding" : {
            "top" : 2147483647,
            "bottom" : 4,
            "left" : 10,
            "right" : -1,
            "inner" : 2147483647
          }
        },
        "txt_t" : {
          "color" : 16777215,
          "font_lib" : "Inter_24pt-ExtraBold.ttf",
          "font_size" : 20
        },
        "img_t" : { }
      } ]
    }
  }, {
    "CLASSNAME" : "image",
    "theme" : "youth",
    "type" : 19,
    "property" : {
      "pub_t" : {
        "name" : "image1",
        "id" : 9,
        "ctrl_type" : 19,
        "pt" : {
          "x" : 160,
          "y" : 195
        },
        "size" : {
          "width" : 378,
          "height" : 360
        },
        "t_mode" : 1,
        "scrollbar_t" : { },
        "alpha" : 255,
        "pub" : true
      },
      "init_img" : "saved-dark.png",
      "rotate_pt" : { }
    }
  } ],
  "view" : {
    "v_name" : "view5",
    "v_id" : 5,
    "bg_scale" : 255,
    "bg_color" : 4278190080
  }
}