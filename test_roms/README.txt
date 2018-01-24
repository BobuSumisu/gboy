# CGB Sound
cgb_sound/cgb_sound.gb                                              FAILED
cgb_sound/rom_singles/01-registers.gb                               FAILED
cgb_sound/rom_singles/02-len ctr.gb                                 FAILED
cgb_sound/rom_singles/03-trigger.gb                                 FAILED
cgb_sound/rom_singles/04-sweep.gb                                   FAILED
cgb_sound/rom_singles/05-sweep details.gb                           FAILED
cgb_sound/rom_singles/06-overflow on trigger.gb                     FAILED
cgb_sound/rom_singles/07-len sweep period sync.gb                   FAILED
cgb_sound/rom_singles/08-len ctr during power.gb                    FAILED
cgb_sound/rom_singles/09-wave read while on.gb                      FAILED
cgb_sound/rom_singles/10-wave trigger while on.gb                   FAILED
cgb_sound/rom_singles/11-regs after power.gb                        FAILED
cgb_sound/rom_singles/12-wave.gb                                    FAILED

# CPU Instructions
cpu_instrs/cpu_instrs.gb                                            OK (But loops?)
cpu_instrs/individual/01-special.gb                                 OK
cpu_instrs/individual/02-interrupts.gb                              OK
cpu_instrs/individual/03-op sp,hl.gb                                OK
cpu_instrs/individual/04-op r,imm.gb                                OK
cpu_instrs/individual/05-op rp.gb                                   OK
cpu_instrs/individual/06-ld r,r.gb                                  OK
cpu_instrs/individual/07-jr,jp,call,ret,rst.gb                      OK
cpu_instrs/individual/08-misc instrs.gb                             OK
cpu_instrs/individual/09-op r,r.gb                                  OK
cpu_instrs/individual/10-bit ops.gb                                 OK
cpu_instrs/individual/11-op a,(hl).gb                               OK

# DMG Sound
dmg_sound/dmg_sound.gb                                              FAILED
dmg_sound/rom_singles/01-registers.gb                               FAILED
dmg_sound/rom_singles/02-len ctr.gb                                 FAILED
dmg_sound/rom_singles/03-trigger.gb                                 FAILED
dmg_sound/rom_singles/04-sweep.gb                                   FAILED
dmg_sound/rom_singles/05-sweep details.gb                           FAILED
dmg_sound/rom_singles/06-overflow on trigger.gb                     FAILED
dmg_sound/rom_singles/07-len sweep period sync.gb                   FAILED
dmg_sound/rom_singles/08-len ctr during power.gb                    FAILED
dmg_sound/rom_singles/09-wave read while on.gb                      FAILED
dmg_sound/rom_singles/10-wave trigger while on.gb                   FAILED
dmg_sound/rom_singles/11-regs after power.gb                        FAILED
dmg_sound/rom_singles/12-wave write while on.gb                     FAILED

# Halt Bug
halt_bug.gb                                                         FAILED

# Instruction Timing
instr_timing/instr_timing.gb                                        FAILED

# Interrupt Time
interrupt_time/interrupt_time.gb                                    FAILED

# Memory Timing
mem_timing/mem_timing.gb                                            FAILED
mem_timing/individual/01-read_timing.gb                             FAILED
mem_timing/individual/02-write_timing.gb                            FAILED
mem_timing/individual/03-modify_timing.gb                           FAILED

# Memory Timing 2
mem_timing-2/mem_timing.gb                                          FAILED
mem_timing-2/rom_singles/01-read_timing.gb                          FAILED
mem_timing-2/rom_singles/02-write_timing.gb                         FAILED
mem_timing-2/rom_singles/03-modify_timing.gb                        FAILED

# OAM Bug
oam_bug/oam_bug.gb                                                  FAILED
oam_bug/rom_singles/1-lcd_sync.gb                                   FAILED
oam_bug/rom_singles/2-causes.gb                                     FAILED
oam_bug/rom_singles/3-non_causes.gb                                 OK
oam_bug/rom_singles/4-scanline_timing.gb                            FAILED
oam_bug/rom_singles/5-timing_bug.gb                                 FAILED
oam_bug/rom_singles/6-timing_no_bug.gb                              OK
oam_bug/rom_singles/7-timing_effect.gb                              FAILED
oam_bug/rom_singles/8-instr_effect.gb                               FAILED
