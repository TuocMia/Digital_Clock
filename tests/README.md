# Unit / Hardware Tests

Thu muc luu cac chuong trinh test doc lap tren kit ma khong can sua `main.c`.

## Danh sach test
- `test_display_button.c`: Test doc lap `T_Button` (PA0, chong doi 30 ms) va `T_Display` + `ui_render` (ST7789 240x240, cac trang thai S0..S5 va thanh tieu de do `ALARM!`).

## Cach build va nap mot bai test
```bash
make clean
make MAIN_SRC=tests/test_display_button.c
make flash
```
*(Build chuong trinh chinh mac dinh chi can chay `make`.)*
