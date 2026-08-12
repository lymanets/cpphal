from pathlib import Path
import os
import click
import yaml
import pdfplumber
import re
import pymupdf
from curl_cffi import requests
import tempfile

@click.command()
@click.argument("headers", type=click.Path(exists=True))
@click.option("-o", "--output", type=click.Path())
def main(headers, output):
    headers = Path(headers)
    files = os.listdir(headers)
    files = [f for f in files if not "f1xx" in f]
    n = len(files)
    c = 0
    for file in files:
        if "f1xx" in file:
            continue
        device = file.replace(".h", "")
        c += 1
        of = f"{output}/{device.lower()}.yaml"
        if os.path.exists(of):
            print(f"[{c}/{n}]Manifest {device} already exists")
            continue
        print(f"[{c}/{n}]Generating manifest for {device}")
        memory = get_stm32f1_ram(device)
        freq = get_stm32f1_freq(device)
        svd = f"{device[:9].upper()}.svd"
        af = build_af_manifest(device)
        generate_manifest(f"{headers}/{file}", device, svd, memory, freq, af, of)

def download_datasheet(family, product_url: str) -> str:
    r = requests.get(
        product_url,
        impersonate="chrome",
    )
    
    with tempfile.NamedTemporaryFile(mode='w+b') as temp_file:
        temp_file.write(r.content)
        return extract_af_tables_stm32f1(family, temp_file.name)

def build_af_manifest(part_number):
    pn = part_number.upper().strip()
        
    if not pn.startswith("STM32F1"):
        raise ValueError("Error: This script only supports the STM32F1 family.")
    
    if len(pn) < 11:
        raise ValueError("Error: Part number is too short to parse.")
    F1_FAMILY_MAP = {
        # STM32F100
        "STM32F100X4": ["STM32F100C4"],
        "STM32F100X6": ["STM32F100C6"],
        "STM32F100X8": ["STM32F100C8", "STM32F100R8", "STM32F100V8"],
        "STM32F100XB": ["STM32F100CB", "STM32F100RB", "STM32F100VB"],
        "STM32F100XC": ["STM32F100RC", "STM32F100VC", "STM32F100ZC"],
        "STM32F100XD": ["STM32F100RD", "STM32F100VD", "STM32F100ZD"],
        "STM32F100XE": ["STM32F100RE", "STM32F100VE", "STM32F100ZE"],

        # STM32F101
        "STM32F101X4": ["STM32F101C4"],
        "STM32F101X6": ["STM32F101C6"],
        "STM32F101X8": ["STM32F101C8", "STM32F101R8", "STM32F101V8"],
        "STM32F101XB": ["STM32F101CB", "STM32F101RB", "STM32F101VB"],
        "STM32F101XC": ["STM32F101RC", "STM32F101VC", "STM32F101ZC"],
        "STM32F101XD": ["STM32F101RD", "STM32F101VD", "STM32F101ZD"],
        "STM32F101XE": ["STM32F101RE", "STM32F101VE", "STM32F101ZE"],
        "STM32F101XF": ["STM32F101RF", "STM32F101VF", "STM32F101ZF"],
        "STM32F101XG": ["STM32F101RG", "STM32F101VG", "STM32F101ZG"],

        # STM32F102
        "STM32F102X4": ["STM32F102C4"],
        "STM32F102X6": ["STM32F102C6"],
        "STM32F102X8": ["STM32F102C8", "STM32F102R8"],
        "STM32F102XB": ["STM32F102CB", "STM32F102RB"],
        "STM32F102XE": ["STM32F102RE", "STM32F102VE"],

        # STM32F103
        "STM32F103X4": ["STM32F103C4"],
        "STM32F103X6": ["STM32F103C6"],
        "STM32F103X8": ["STM32F103C8", "STM32F103R8", "STM32F103V8"],
        "STM32F103XB": ["STM32F103CB", "STM32F103RB", "STM32F103VB"],
        "STM32F103XC": ["STM32F103RC", "STM32F103VC", "STM32F103ZC"],
        "STM32F103XD": ["STM32F103RD", "STM32F103VD", "STM32F103ZD"],
        "STM32F103XE": ["STM32F103RE", "STM32F103VE", "STM32F103ZE"],
        "STM32F103XF": ["STM32F103RF", "STM32F103VF", "STM32F103ZF"],
        "STM32F103XG": ["STM32F103RG", "STM32F103VG", "STM32F103ZG"],

        # STM32F105
        "STM32F105X8": ["STM32F105R8", "STM32F105V8"],
        "STM32F105XB": ["STM32F105RB", "STM32F105VB"],
        "STM32F105XC": ["STM32F105RC", "STM32F105VC"],

        # STM32F107
        "STM32F107XB": ["STM32F107RB", "STM32F107VB"],
        "STM32F107XC": ["STM32F107RC", "STM32F107VC"],
    }
    if not pn in F1_FAMILY_MAP:
        raise ValueError(f"{pn} is not in map")
    device = F1_FAMILY_MAP[pn][0].lower()
    family_code = pn[6:9]
    url = f"https://www.st.com/resource/en/datasheet/{device}.pdf"
    table = download_datasheet(family_code, url)


    return parse_pin_table(table)


def extract_af_tables_stm32f1(family, pdf_path: str):
    """
    Extract tables containing alternate function mappings.

    Returns:
        [
            {
                "page": 42,
                "headers": [...],
                "rows": [...]
            },
            ...
        ]
    """

    tables = []

    with pdfplumber.open(pdf_path) as pdf:
        doc = pymupdf.open(pdf_path)
        toc = doc.get_toc()

        # Find "Alternate function mapping"
        if family == "105" or family == "107":
            page_title = "Pin definitions"
        else:
            page_num, page_title = next(
                (page, title) for _, title, page in toc
                if "pin definition" in title.lower()# or ((family == "105" or family == "107") and "pinouts and pin description" in title.lower())
            )
        page_title = page_title.replace("(continued)", "").strip()
        c = 0
        start_page = None
        end_page = None
        for page in pdf.pages:
            lines = page.extract_text_lines()
            c += 1
            if len(lines) < 2:
                continue
            if not page_title in lines[1]["text"]:
                if end_page is None and start_page is not None:
                    end_page = c
                    break
                continue
            if start_page is None:
                start_page = c

        if start_page is None and end_page is None:
            raise ValueError("Can not find table")
        pages = pdf.pages[start_page-1:end_page-1]
        
        pin_name = None
        default_function = None
        remap_function = None
        for page in pages:
            for table in page.extract_tables():
                if not table:
                    continue
                
                headers = table[:2]
                
                if headers is None:
                    continue

                headers = [[str(c) for c in t] for t in headers]
                if default_function is None and "Alternatefunctions" in "".join(headers[0]).replace(" ", ""):
                    c = 0
                    for f in headers[0]:
                        if "Alternate functions" in f or "Alternatefunctions" in f:
                            break
                        c += 1
                    if c != len(headers[0]):
                        idx = c
                        default_function = idx
                        remap_function = idx + 1
                if pin_name is None and "Pin name" in "".join(headers[0]):
                    pin_name = headers[0].index("Pin name")

                tables += table[2:]
            if default_function is None:
                raise ValueError("Default column is not found")
    return  {
        "pin_name": pin_name, 
        "default_function": default_function, 
        "remap_function": remap_function,
        "rows": tables
    }

def test_and_fix(val):
    if val is None or val == "-" or len(val) == 0:
        return ""
    return val

def fix_name(val, no_new_line=True):
    val = re.sub(r'\n?\(\n?\d+\n?\)', '', val)
    val = re.sub(r'(?<=_[A-Z0-9])\n(?=[A-Z0-9])', '', val)
    return val

# def get_list_of_af(val):
#     val = fix_name(val, no_new_line=False).replace("_\n", "_").replace("\n_", "_").split("\n")
#     ret = []
#     for v in val:
#         r = v.split("/")
#         ret += r
#     ret = [v.strip() for v in ret if len(v.strip()) != 0]

#     return ret

import re

def merge_split_identifiers(lines: list[str]) -> list[str]:
    result = []

    for line in map(str.strip, lines):
        if not result or "MCO" == line:
            result.append(line)
            continue

        prev = result[-1]

        # TIM2_CH1_ + ETR -> TIM2_CH1_ETR
        if prev.endswith("_") and re.fullmatch(r"[A-Z0-9_]+", line):
            result[-1] += line
            continue

        # USART3_CT + S -> USART3_CTS
        # USART2_RT + S -> USART2_RTS
        if (
            re.search(r"_[A-Z0-9]+$", prev)
            and re.fullmatch(r"[A-Z0-9]{1,3}", line)
        ):
            result[-1] += line
            continue

        result.append(line)

    return result

def get_list_of_af(val):
    val = fix_name(val)

    val = merge_split_identifiers([
        x.strip()
        for x in re.split(r"[/\n]", val)
        if x.strip()
    ])

    val = [v.split("_") for v in val]
    val = [{"peripheral": v[0], "signal": v[1]} for v in val if len(v) == 2]

    return val


def parse_pin_table(table):
    pins = {}
    pin_name_idx = table["pin_name"]
    default_function_idx = table["default_function"]
    remap_function_idx = table["remap_function"]

    for row in table["rows"]:

        pin = fix_name(row[pin_name_idx]).split("-")[0]
        if not pin:
            continue

        def_function = row[default_function_idx]
        remap_function = row[remap_function_idx]
        def_function = test_and_fix(def_function)
        remap_function = test_and_fix(remap_function)

        if len(def_function) == 0 and len(remap_function) == 0:
            continue

        
        def_function = get_list_of_af(def_function)
        remap_function = get_list_of_af(remap_function)

        pins[pin] = {
                "functions": def_function,
                "remap": remap_function
            }

    return pins

def get_stm32f1_freq(part_number):
    pn = part_number.upper().strip()
    
    if not pn.startswith("STM32F1"):
        raise ValueError("Error: This script only supports the STM32F1 family.")
    
    if len(pn) < 11:
        raise ValueError("Error: Part number is too short to parse.")
        
    freq_map = {
        "100": {
            "sysclk": 24,
            "apb1": 24,
            "apb2": 24,
        },
        "101": {
            "sysclk": 36,
            "apb1": 36,
            "apb2": 36,
        },
        "102": {
            "sysclk": 48,
            "apb1": 24,
            "apb2": 48,
        },
        "103": {
            "sysclk": 72,
            "apb1": 36,
            "apb2": 72,
        },
        "105": {
            "sysclk": 72,
            "apb1": 36,
            "apb2": 72,
        },
        "107": {
            "sysclk": 72,
            "apb1": 36,
            "apb2": 72,
        }

    }

    family_code = pn[6:9]
    return freq_map[family_code]

def get_stm32f1_ram(part_number):
    pn = part_number.upper().strip()
    
    if not pn.startswith("STM32F1"):
        raise ValueError("Error: This script only supports the STM32F1 family.")
    
    if len(pn) < 11:
        raise ValueError("Error: Part number is too short to parse.")
        
    size_code = pn[10]
    family_code = pn[6:9]
    
    # Map the naming convention code to Flash size in KB
    flash_map = {
        "4": {"flash": 16,"103_ram": 6,"102_ram": 4,"101_ram": 4,"100_ram": 4},
        "6": {"flash": 32,"103_ram": 10,"102_ram": 6,"101_ram": 6,"100_ram": 4},
        "8": {"flash": 64,"103_ram": 20,"102_ram": 10,"101_ram": 10,"100_ram": 8},
        "B": {"flash": 128,"103_ram": 20,"102_ram": 16,"101_ram": 16,"100_ram": 8},
        "C": {"flash": 256,"103_ram": 48,"102_ram": None,"101_ram": 32,"100_ram": 24},
        "D": {"flash": 384,"103_ram": 64,"102_ram": None,"101_ram": 48,"100_ram": 32},
        "E": {"flash": 512,"103_ram": 64,"102_ram": None,"101_ram": 48,"100_ram": 32},
        "F": {"flash": 768,"103_ram": 96,"102_ram": None,"101_ram": 80,"100_ram": None},
        "G": {"flash": 1024,"103_ram": 96,"102_ram": None,"101_ram": 80,"100_ram": None}
    }
    
    if size_code not in flash_map:
        raise ValueError(f"Error: Unknown Flash size code '{size_code}'.")
        
    memory = flash_map[size_code]
    ram_kb = 64 if "105" in pn or "107" in pn else memory[f"{family_code}_ram"]
    return {
        "part": pn,
        "flash": memory["flash"],
        "ram": ram_kb
    }

def generate_manifest(header, device, svd, memory, freq, af, output):
    data = __read_file(header).decode(errors="ignore").splitlines()
    start_idx = -1
    last_idx = -1
    c = 0
    for line in data:
        if start_idx != -1 and "@}" in line:
            last_idx = c
            break
        if "Peripheral_declaration" in line:
            start_idx = c
        
        c +=1
    if start_idx == -1 and last_idx == -1:
        raise ValueError("can not find peripherals")
    peripherals = data[start_idx+3:last_idx-1]
    start_idx = -1
    last_idx = -1
    c = 0
    for line in data:
        if start_idx != -1 and "IRQn_Type" in line:
            last_idx = c
            break
        if "specific Interrupt Numbers" in line:
            start_idx = c
        
        c +=1
    if start_idx == -1 and last_idx == -1:
        raise ValueError("can not find IRQ")
    
    ret = {}
    ret["name"] = device
    ret["svd"] = svd
    irq = data[start_idx+1:last_idx]
    irq = [i.split("/*")[0] for i in irq if "=" in i]
    irq = [i.split(",")[0] for i in irq]
    irq = [i.replace(" ", "").split("=") for i in irq]
    irq = [{int(i[1]):[i[0].replace("_IRQn", "")]} for i in irq]
    irq_final = {}
    for i in irq:
        irq_final.update(i)
    peripherals = [p.split(" ")[1] for p in peripherals if len(p) > 0]
    ret["peripherals"] = peripherals
    ret["interrupts"] = irq_final
    ret["af"] = af
    ret["freq"] = freq

    ret["memory"] = [
        {
            "name": "FLASH",
            "origin": 0x08000000,
            "length": str(memory["flash"]) + "K",
            "attributes": "rx"
        },
        {
            "name": "RAM",
            "origin": 0x20000000,
            "length": str(memory["ram"]) + "K",
            "attributes": "rwx"
        }
    ]

    with open(str(output), "wt") as f:
        yaml.dump(ret, f, sort_keys=False, default_flow_style=False, indent=4)

def __read_file(file):
    with open(str(file), "rb") as f:
        return f.read()


if __name__ == "__main__":
    main()
