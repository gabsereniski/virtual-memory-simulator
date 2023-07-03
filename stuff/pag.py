class Memory:
    def __init__(self, ram_size, page_size):
        self.ram_size = ram_size # Tamanho da RAM
        self.page_size = page_size # Tamanho da página
        self.page_table = {} # Tabela de páginas
        self.page_frames = [] # Frames para os endereços físicos
        self.next_frame_index = 0 # Indice para o próximo frame

    def access_address(self, address, algorithm):
        page_number = address // self.page_size # Arredonda pra baixo (Floor)
        offset = address % self.page_size # Calcula o offset do endereço

        # Se a página estiver na tabela de páginas
        if page_number in self.page_table:
            frame_number = self.page_table[page_number] # Frame = Numero da página
            physical_address = frame_number * self.page_size + offset # Calcula o endereço físico
            print(f"Endereço Virtual: {address} / Endereço Físico: {physical_address}")
        # Se a página não estiver na tabela de páginas
        else:
            print(f"Page Fault: Página {page_number} não está na memória.")
            self.handle_page_fault(page_number, offset, algorithm) # Cuida da page fault

    def handle_page_fault(self, page_number, offset, algorithm):
        
        # Se tiver memória disponível:
        if len(self.page_frames) < self.ram_size // self.page_size:
            # Page fault mas tem frames disponíveis
            frame_number = self.next_frame_index # Adquire o indice do próximo frame
            self.page_table[page_number] = frame_number # Adiciona à tabela de páginas
            self.page_frames.append(page_number) # Adiciona aos frames
            self.next_frame_index += 1 # Aumenta o indice do próximo frame
        else:
            # Page fault e todos os frames estão ocupados, ou seja, ocorre substituição
            ########### FIFO ###########
            if algorithm == "FIFO":
                frame_number = self.page_frames.pop(0) # Seleciona e remove o número do frame que será substituído na memória física.
                del self.page_table[list(self.page_table.keys())[list(self.page_table.values()).index(frame_number)]] # Retira o primeiro que "entrou" na tabela de páginas
                self.page_table[page_number] = frame_number # Substitui a página.
                self.page_frames.append(page_number) # Atribuí a pagina ao frame.
            ########### LRU ###########
            ########### ??? ###########

        physical_address = frame_number * self.page_size + offset # Calcula o endereço físico
        print(f"Endereço Virtual: {page_number*self.page_size + offset}, Endereço Físico: {physical_address}")


# Execução
def simulate_virtual_memory(ram_size, page_size, process_size, algorithm, process):
    memory = Memory(ram_size, page_size)

    print(f"RAM Size: {ram_size} bytes")
    print(f"Page Size: {page_size} bytes")
    print(f"Process Size: {process_size} bytes")
    print(f"Algorithm: {algorithm}")
    print(f"Process: {process}")

    print("\n--- Accessing Addresses ---")
    for address in process:
        memory.access_address(address, algorithm)


# Parâmetros para execução
ram_size = 1024  # Tamanho da RAM em bytes
page_size = 64  # Tamanho das páginas em bytes
process_size = ram_size + 100  # Tamanho do processo (maior que a RAM)

algorithm = "FIFO"  # Algoritmo de substituição de página

process = [
    0x100, 0x200, 0x300, 0x400, 0x500, 0x600, 0x700, 0x800, 0x900, 0xA00,  # Páginas mapeadas inicialmente
    0x100, 0x900, 0x200, 0x600, 0x300, 0x400, 0x500, 0x700, 0x800, 0xB00,  # Páginas mapeadas e desmapeadas
    0x800, 0x1000, 0x900, 0x200, 0x300, 0x400, 0x600, 0xC00, 0x500, 0x700  # Páginas mapeadas e desmapeadas
]

simulate_virtual_memory(ram_size, page_size, process_size, algorithm, process)
