fixed_step =  ['gsl_odeiv2_step_rk2', 'gsl_odeiv2_step_rk4' ]
variable_step = ['gsl_odeiv2_step_rkf45', 'gsl_odeiv2_step_rkck', 'gsl_odeiv2_step_rk8pd', \
                   'gsl_odeiv2_step_rk2imp', 'gsl_odeiv2_step_rk4imp', 'gsl_odeiv2_step_bsimp', 'gsl_odeiv2_step_rk1imp', \
                   'gsl_odeiv2_step_msadams', 'gsl_odeiv2_step_msbdf']
embedded =  ['standard RK4']

dictTemplates = {
                 'sim.tmf' : embedded + fixed_step + variable_step,
                 'fmusim.tmf' : embedded + fixed_step + variable_step,
                 'rt_nrt_iopl.tmf': embedded + fixed_step,
                 'rt.tmf' : embedded + fixed_step,
                 'fmurt.tmf' : embedded + fixed_step,
                 'nuttx_timehook.tmf' : embedded,
                 'nuttx.tmf' : embedded,
                 'rt_co.tmf' :  embedded + fixed_step,
                 'rt_digi.tmf' :  embedded + fixed_step,
                 'rt_mz_apo.tmf' : embedded,
                 'rt_nrt_iopl.tmf' : embedded + fixed_step,
                 'rt_pi.tmf' : embedded,
                 'rt.tmf' : embedded + fixed_step,
                 'samd21DBG.tmf' : embedded,
                 'samd21.tmf' : embedded,
                 'stm32h7.tmf' : embedded,
                 }


