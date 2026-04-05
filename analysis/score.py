import pandas as pd

picks   = pd.read_csv("data/pick_data.csv")
dist_df = pd.read_csv("data/dist_matrix.csv", index_col=0)
sub     = pd.read_csv("data/example_submission.csv")

merged = picks.merge(sub, on=["ORDER_ID", "WAVE_ID"])

total = 0
for list_id, group in merged.groupby("LIST_ID"):
    locs = group.drop_duplicates("LOCATION_ID").sort_values("ASIS_SEQUENCE")["LOCATION_ID"].tolist()
    route = ["START"] + locs + ["END"]
    total += sum(dist_df.loc[route[i], route[i+1]] for i in range(len(route)-1))

print(f"Baseline score: {total:.2f}")