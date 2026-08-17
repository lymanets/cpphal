
from dataclasses import dataclass, field
from cmsis_svd.model import SVDDevice, SVDPeripheral

@dataclass(slots=True)
class GenerationContext:
    device: SVDDevice
    rcc: SVDPeripheral|None
    exti: SVDPeripheral|None
    syscfg: SVDPeripheral|None

    @classmethod
    def from_device(cls, device:SVDDevice):
        def find(name):
            return next((p for p in device.peripherals if p.name==name), None)
        return cls(
            device=device,
            rcc=find("RCC"),
            exti=find("EXTI"),
            syscfg=find("SYSCFG"),
        )

    def get_cpu_name(self) -> str:
       return "cortex-" + self.device.cpu.name.name.lower().replace("cm", "m")

@dataclass(slots=True)
class PeripheralContext:
    generation: GenerationContext
    peripheral: SVDPeripheral
    clock_register:str|None=None
    clock_tag:str|None=None
    gpio_policy:str|None=None
    clock_bit:int|None=None
    reset_register:str|None=None
    reset_bit:int|None=None
    irq_number:int|None=None
    is_gpio:bool=False
    is_rcc:bool=False
    is_uart:bool=False
    is_spi:bool=False
    is_i2c:bool=False
    signals:list|None=None
    event_namespace:str|None=None
    events:list|None=None
    gpio_inc:list|None=None
    traits_namespace:str=""
    extra:dict=field(default_factory=dict)

    def get_access(self, access) -> str: 
        return "".join([a.capitalize() for a in access.value.split("-")])
