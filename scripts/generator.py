
from cmsis_svd import SVDParser
from .context import GenerationContext
from .builder import make_context
from .renderer import create_environment
from .renderers import DeviceRenderer, PeripheralRenderer, DeviceCmakeRenderer, OptionsRenderer, TagsRenderer, DocsRenderer, EventsRenderer, LinkerRenderer
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
    peripheral_options = __load_yaml(configs/"peripheral.yaml")
    doc_dir = configs.parent/"docs"
    svd = manifest["svd"]
    device = SVDParser.for_xml_file(f"{svd_dir}/{svd[:7]}/{svd}").get_device()
    gen = GenerationContext.from_device(device)
    if os.path.exists(output):
        shutil.rmtree(output)

    env = create_environment()
    inc_output = output/"include/"
    peripherals = manifest["peripherals"]
    interrupts = manifest["interrupts"]
    irq_count = max(list(interrupts.keys()))
    device.peripherals = [p for p in device.peripherals if p.name in peripherals]
    DeviceRenderer(env).render_device(
        device,
        manifest["freq"],
        namespace,
        irq_count,
        manifest["memory"],
        inc_output,
    )

    DeviceCmakeRenderer(env).render_device(
        gen,
        namespace,
        output,
    )

    OptionsRenderer(env).render_options(
        peripheral_options,
        output,
    )

    TagsRenderer(env).render_tags(
        peripheral_options,
        output,
    )

    EventsRenderer(env).render_events(
        event_config,
        output,
    )

    LinkerRenderer(env).render_linker(
        manifest["memory"],
        output,
    )

    DocsRenderer().render_doc(env, event_config,  peripheral_options, doc_dir)

    renderer = PeripheralRenderer(env)

    for peripheral in device.peripherals:
        ctx = make_context(gen, signals_config, event_config, manifest["af"], peripheral)
        renderer.render_peripheral(
            ctx,
            namespace,
            inc_output,
        )
