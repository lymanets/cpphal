
from cmsis_svd import SVDParser
from .context import GenerationContext
from .builder import make_context
from .renderer import create_environment
from .renderers import DeviceRenderer, PeripheralRenderer, DeviceCmakeRenderer, VectorsRenderer, LinkerRenderer
import yaml
import os
import shutil

def __load_yaml(file):
    with open(file, "r") as f:
        return yaml.safe_load(f)

def generate(configs, manifest, svd_dir, output, namespace):
    manifest = __load_yaml(manifest)
    signals_config = __load_yaml(configs/"signal.yaml")
    event_config = __load_yaml(configs/"event.yaml")
    svd = manifest["svd"]
    device = SVDParser.for_xml_file(f"{svd_dir}/{svd[:7]}/{svd}").get_device()
    gen = GenerationContext.from_device(device)
    if os.path.exists(output):
        shutil.rmtree(output)

    env = create_environment()
    inc_output = output/"include/"
    peripherals = manifest["peripherals"]
    interrupts = manifest["interrupts"]
    device.peripherals = [p for p in device.peripherals if p.name in peripherals]
    DeviceRenderer(env).render_device(
        device,
        manifest["freq"],
        namespace,
        inc_output,
    )

    DeviceCmakeRenderer(env).render_device(
        gen,
        namespace,
        output,
    )

    VectorsRenderer(env).render_vectors(
        device,
        interrupts,
        output,
    )

    LinkerRenderer(env).render_linker(
        manifest["memory"],
        output,
    )

    renderer = PeripheralRenderer(env)

    for peripheral in device.peripherals:
        ctx = make_context(gen, signals_config, event_config, manifest["af"], peripheral)
        renderer.render_peripheral(
            ctx,
            namespace,
            inc_output,
        )
