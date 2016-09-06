#include <vpi_user.h>

#define N_LED (18)
#define N_SW  (18)
#define N_BTN (4)

// For vector represenation, see "The Verilog PLI Handbook" section 5.2.7 "Reading Verilog 4-state logic vectors as encoded aval/bval pairs" (1st edition page 150, 2nd edition page 163)

//////// LEDS ////////

PLI_INT32
DE2_leds_compiletf(PLI_BYTE8 *user_data)
{
	vpiHandle systf, iter, arg;

	systf = vpi_handle(vpiSysTfCall, NULL);
	iter = vpi_iterate(vpiArgument, systf);
	if (iter == NULL) // must have args
		goto fail;

	arg = vpi_scan(iter);
	if (arg == NULL) // should not be reachable... but just in case
		goto fail;

	if (vpi_get(vpiType, arg) != vpiNet) // arg must be vec[]
		goto fail;

	if (vpi_get(vpiSize, arg) != N_LED) // vec must be N_LED bits wide
		goto fail;

	if (vpi_scan(iter) != NULL) // must not have >1 arg
		goto fail;

	return 0;

	// We are leaking an iter in some cases, but vpi_free_object(iter) double-frees if used after vpi_scan returns NULL and frees non-alloc'd if used before scanning, so it's significantly more readable (and non-problematic since in _complie) to just leak. See handbook section 4.5.3 "When to use vpi_free_object() on iterator handles" (2nd edition page 108)
fail:
	vpi_printf("ERROR: $DE2_leds() requires exactly one %d-bit wide vector argument\n", N_LED);
	vpi_control(vpiFinish, 1);
	return 0;
}

PLI_INT32
DE2_leds_calltf(PLI_BYTE8 *user_data)
{
	vpiHandle systf, iter, vec;
	s_vpi_value val;
	PLI_INT32 aval, bval;

	systf = vpi_handle(vpiSysTfCall, NULL);
	iter = vpi_iterate(vpiArgument, systf);
	vec = vpi_scan(iter);
	vpi_free_object(iter);

	val.format = vpiVectorVal;
	vpi_get_value(vec, &val);
	aval = val.value.vector[0].aval;
	bval = val.value.vector[0].bval;

	vpi_printf("%x %x\n", aval, bval);

	return 0;
}

void
DE2_leds_register(void)
{
	s_vpi_systf_data tf_data;

	tf_data.type = vpiSysTask;
	tf_data.sysfunctype = 0;
	tf_data.tfname = "$DE2_leds";
	tf_data.calltf = DE2_leds_calltf;
	tf_data.compiletf = DE2_leds_compiletf;
	tf_data.sizetf = NULL;
	tf_data.user_data = NULL;

	vpi_register_systf(&tf_data);
}

//////// SWITCHES ////////

PLI_INT32
DE2_switches_calltf(PLI_BYTE8 *user_data)
{
	vpiHandle systf;
	s_vpi_value val;
	s_vpi_vecval vec0;

	systf = vpi_handle(vpiSysTfCall, NULL);

	val.format = vpiVectorVal;
	val.value.vector = &vec0;
	vec0.aval = (PLI_INT32)(((1<<N_SW)-1) & 0x55555555);
	vec0.bval = 0;
	vpi_put_value(systf, &val, NULL, vpiNoDelay);

	return 0;
}

PLI_INT32
DE2_switches_sizetf(PLI_BYTE8 *user_data)
{
	return (N_SW);
}

void
DE2_switches_register(void)
{
	s_vpi_systf_data tf_data;

	tf_data.type = vpiSysFunc;
	tf_data.sysfunctype = vpiSizedFunc;
	tf_data.tfname = "$DE2_switches";
	tf_data.calltf = DE2_switches_calltf;
	tf_data.compiletf = NULL;
	tf_data.sizetf = DE2_switches_sizetf;
	tf_data.user_data = NULL;

	vpi_register_systf(&tf_data);
}

//////// BUTTONS ////////

PLI_INT32
DE2_buttons_calltf(PLI_BYTE8 *user_data)
{
	vpiHandle systf;
	s_vpi_value val;
	s_vpi_vecval vec0;

	systf = vpi_handle(vpiSysTfCall, NULL);

	val.format = vpiVectorVal;
	val.value.vector = &vec0;
	vec0.aval = (PLI_INT32)(((1<<N_BTN)-1) & 0xAAAAAAAA);
	vec0.bval = 0;
	vpi_put_value(systf, &val, NULL, vpiNoDelay);

	return 0;
}

PLI_INT32
DE2_buttons_sizetf(PLI_BYTE8 *user_data)
{
	return (N_BTN);
}

void
DE2_buttons_register(void)
{
	s_vpi_systf_data tf_data;

	tf_data.type = vpiSysFunc;
	tf_data.sysfunctype = vpiSizedFunc;
	tf_data.tfname = "$DE2_buttons";
	tf_data.calltf = DE2_buttons_calltf;
	tf_data.compiletf = NULL;
	tf_data.sizetf = DE2_buttons_sizetf;
	tf_data.user_data = NULL;

	vpi_register_systf(&tf_data);
}

//////// VPI REGISTRATION ////////

void (*vlog_startup_routines[])() = {
	DE2_leds_register,
	DE2_switches_register,
	DE2_buttons_register,
	0
};
