set(COMPONENT_SRCS
    "main.c"
    "control.c"
    "board.c"
    "board_ps.c"
    "board_fs.c"
    "board_sw.c"
    "link.c"
    "hal.c"
    "drv_i2c_ms5525dso.c"
    "drv_i2c_tca9548a.c"
    "drv_i2c_sfm3000.c"
)

set(COMPONENT_ADD_INCLUDEDIRS
    "."
)

register_component()
