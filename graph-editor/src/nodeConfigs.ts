import { NodeConfig } from "./types";

const nodeConfigs: Record<string, NodeConfig> = {
  adder: {
    name: "Adder",
    config: { inputs: 2, outputs: 1, saturating: false }
  },
  multiplier: {
    name: "Multiplier",
    config: { inputs: 2, outputs: 1, scale_factor: 1.0 }
  },
  constant: {
    name: "Constant",
    config: { inputs: 0, outputs: 1, value: 0 }
  }
};

export default nodeConfigs;

