export interface NodeConfig {
  name: string;
  config: Record<string, number | boolean | string>;
}

export interface GraphNode {
  id: string;
  type: string;
  position: { x: number; y: number };
  data: {
    label: string;
    config: Record<string, number | boolean | string>;
  };
}

