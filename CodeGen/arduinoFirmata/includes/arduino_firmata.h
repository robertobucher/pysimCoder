void setup_af(int Flag, python_block *block);
void digitalWrite_af(int flag, python_block *block);
void analogWrite_af(int flag, python_block *block);
void pinRead_af(int flag, python_block *block);

// helper blocks
void help_HeatShield_temperature(int flag, python_block *block);
void help_TCLab_temperature(int flag, python_block *block);

extern t_firmata * af_firmataInstance;
