using Plots
using Weave

using Plots

xs = range(0, 2π, length = 10)
data = [sin.(xs) cos.(xs) 2sin.(xs) 2cos.(xs)]

labels = ["Apples" "Oranges" "Hats" "Shoes"]

markershapes = [:circle, :star5]

markercolors = [
    :green :orange :black :purple
    :red   :yellow :brown :white
]

plot(
    xs,
    data,
    label = labels,
    shape = markershapes,
    color = markercolors,
    markersize = 10
)