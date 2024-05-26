# make ARCH=riscv32e-nemu run
#
echo -e "\e[31m if difftest error at ecall, it's because other inst write back at WB stage, but csr write back at EX stage, not a big deal!! \e[0m"
make ARCH=riscv32e-npc run

# make ARCH=native run
