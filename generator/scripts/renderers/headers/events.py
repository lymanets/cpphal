from scripts.renderers.base import BaseRenderer

class EventsRenderer(BaseRenderer):
    template_name="headers/events.hpp.j2"

    def render_events(self, events, output):
        for name, options in events.items():
            new_name = name.replace("*", "").lower()
            new_name = "uart" if new_name == "usart" else "timer" if new_name == "tim" else new_name
            header_dir = output/"include"/"hal"/f"{new_name}"
            self.render_to_file(header_dir/"events.hpp",
                                events=options.keys(),
                                namespace=new_name)
