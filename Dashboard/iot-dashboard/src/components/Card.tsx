import { ReactNode, useEffect, useState, useTransition } from 'react'
import { Button } from "@/components/ui/button"
import {
  Dialog,
  DialogContent,
  DialogDescription,
  DialogFooter,
  DialogHeader,
  DialogTitle,
  DialogTrigger,
} from "@/components/ui/dialog"
import {
  Form,
  FormControl,
  FormDescription,
  FormField,
  FormItem,
  FormLabel,
  FormMessage,
} from "@/components/ui/form"
import { Input } from "@/components/ui/input"
import { LineChartData } from './charts/LineChartData'
import { z } from 'zod'
import { useForm } from 'react-hook-form'
import { zodResolver } from '@hookform/resolvers/zod'
import { addRange, getRanges, updateRanges } from '@/actions'
import { useToast } from '@/hooks/use-toast'
import { Icons } from '@/components/icons'
import { cn } from '@/lib/utils'

type SensorData = {
  time: string,
  value: number
}
interface CardProps {
  title: string
  icon: ReactNode
  value: string
  unit: string
  texts: string[]
  chartData: SensorData[]
  predictData: SensorData[]
  deviceId: string
}

const FormSchema = z.object({
  minValue: z.string().min(1, {
    message: 'Min value must be at least 1 number.'
  }),
  maxValue: z.string().min(1, {
    message: 'Max value must be at least 1 number.'
  }),
})

export function Card({ title, icon, value, unit, texts, chartData, predictData, deviceId }: CardProps) {
  const { toast } = useToast()
  const [isPending, startTransition] = useTransition()
  const [isOpen, setIsOpen] = useState<boolean>(false)


  const form = useForm<z.infer<typeof FormSchema>>({
    resolver: zodResolver(FormSchema),
    defaultValues: {
      minValue: '',
      maxValue: ''
    },
  })

  function onSubmit(values: z.infer<typeof FormSchema>) {
    startTransition(async () => {
      const { minValue, maxValue } = values

      const check = (await getRanges(title, deviceId)).data

      const task = {
        deviceId,
        sensor: title,
        minValue: parseFloat(minValue),
        maxValue: parseFloat(maxValue),
        isCreatedByUser: true
      }

      const { data, error } = (check?.length > 0) ? (await updateRanges(deviceId, task, title)) : (await addRange(task))

      if (error) {
        console.log('error >>>>>', error)
      }
      else {
        toast({
          title: "You submitted the following values:",
          description: (
            <pre className="mt-2 w-[340px] rounded-md bg-slate-950 p-4">
              <p className="text-white">Submitted Successfully</p>
            </pre>
          ),
        })
      }

      form.reset()

      setIsOpen(false)
    })
  }

  useEffect(() => {
    const test = async () => {

    }

    test()
  }, [])

  return (

    <div className="flex w-full" >
      <Dialog open={isOpen} onOpenChange={(open) => {
        setIsOpen(open)
      }}>
        <DialogTrigger asChild>
          <div className="flex flex-col w-full rounded-xl p-3  bg-gradient-to-tl from-[#0e1426] from-1% to-70% to-[#1d2b53] min-h-52 transform transition-transform ">
            <div className="flex justify-between h-1/4 ">
              <h3 className="text-xl roboto">{title}</h3>
              <div className="w-[30px] h-[30px] rounded-full bg-[#0e1426] p-1">
                {icon}
              </div>
            </div>
            <div className="flex h-full justify-center items-center">
              <div className="flex items-start">
                <h1 className="text-4xl md:text-6xl">{value}</h1>
                <p className="ordinal">{unit}</p>
              </div>
            </div>
            <div className="flex flex-col h-1/4">
              <div className="w-full h-[2px] bg-gradient-to-r from-transparent via-[#344378] to-transparent"></div>
              <div className="flex justify-between items-center mt-2">
                {texts.map((text, index) => (
                  <p key={index} className="text-[#5a678c]">
                    {text}
                  </p>
                ))}
              </div>
            </div>
          </div>
        </DialogTrigger>
        <DialogContent className="sm:max-w-[40%]">
          <DialogHeader>
            <DialogTitle>{title}</DialogTitle>
            <DialogDescription>
              {texts.map((text, index) => (
                <p key={index} className="text-[#5a678c]">
                  {text}
                </p>
              ))}
            </DialogDescription>
          </DialogHeader>
          <Form {...form}>
            <form onSubmit={form.handleSubmit(onSubmit)} className="space-y-2">
              <FormField
                control={form.control}
                name="minValue"
                render={({ field }) => (
                  <FormItem className='flex justify-between gap-4 items-center'>
                    <FormLabel className='min-w-12'>Min Value</FormLabel>
                    <FormControl>
                      <Input placeholder="1" {...field} />
                    </FormControl>
                    {/* <FormDescription>
                      This is your public display name.
                    </FormDescription> */}
                    <FormMessage />
                  </FormItem>
                )}
              />
              <FormField
                control={form.control}
                name="maxValue"
                render={({ field }) => (
                  <FormItem className='flex justify-between gap-4 items-center'>
                    <FormLabel>Max Value</FormLabel>
                    <FormControl>
                      <Input placeholder="99" {...field} type='number' />
                    </FormControl>
                    {/* <FormDescription>
                      This is your public display name.
                    </FormDescription> */}
                    <FormMessage />
                  </FormItem>
                )}
              />
              <Button type="submit" className='w-full'>Submit <Icons.spinner
                className={cn('animate-spin', { hidden: !isPending })}
              /></Button>
            </form>
          </Form>
          {/* <div className="grid gap-4 py-4">
            <div className="flex justify-between gap-4 items-center">
              <Label htmlFor="min" className="text-left min-w-[10%]">
                Min Value
              </Label>
              <Input id="min" />
            </div>
          </div> */}
          {/* <Button onClick={async () => {
            const token = global?.window?.localStorage.getItem('token') ?? ''

            const data = await getHistoricalDataTb(token, deviceId, 'pH', (Date.now() - 7 * 24 * 60 * 60 * 1000).toString(), (Date.now()).toString(), '1800000', 'AVG', '1000')

            console.log('data >>>>', data)
          }} className='w-full'>Get data</Button> */}
          <LineChartData realData={chartData} predictData={predictData} title={title} deviceId={deviceId} />
        </DialogContent>
      </Dialog>
    </div>
  )
}
