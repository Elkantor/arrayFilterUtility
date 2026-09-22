## How It Works

`Filtered Array` packs all entities into a single contiguous array (`data`) and uses an array of indices (`cursors`) to keep track of state boundaries without pre-allocating fixed sub-buffers.

### Memory Layout

For `NUM_STATES = 3` and `CAPACITY = 8`:

```text
cursors[] = [  0,        2,              5,               6,        6  ]
               │         │               │                │         │
               ▼         ▼               ▼                ▼         ▼
Index:         0    1    2    3    4     5                6    7
            ┌────┬────┬────┬────┬────┬───────┬──────────────────────┐
data[]:     │ A1 │ A2 │ B1 │ B2 │ B3 │  C1   │  Unused Capacity...  │
            └────┴────┴────┴────┴────┴───────┴──────────────────────┘
            └─────────┴──────────────┴───────┴──────────────────────┘
              State 0     State 1     State 2   State 3 (empty)
             [0..1]      [2..4]       [5..5]     [6..6]
