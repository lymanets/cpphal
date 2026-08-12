from .base import BaseRenderer

class DeviceCmakeRenderer(BaseRenderer):
    template_name="device.cmake.j2"

    def render_device(self, ctx, namespace, output):
        self.render_to_file(output/"device.cmake",
                            ctx=ctx,
                            namespace=namespace)
